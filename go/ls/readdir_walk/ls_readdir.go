package main

import (
	"fmt" // filesystem errors
	"os"
	"os/user"
	"path/filepath" // join
	"sort"
	"strconv"
	"strings" // assist sort funcs
	"syscall"
	"time"
)

// Options
const (
	LS_PHYS    uint32 = 1 << 0
	LS_MOUNT          = 1 << 1
	LS_NODIR          = 1 << 2
	LS_ONLYDIR        = 1 << 3
)

type Xstat func(string, *syscall.Stat_t) error

// Syscall mapping to different stat calls
// Used to quickly resolve LS_PHYS without
// using a conditional
var smap = map[uint32]Xstat{
	0: syscall.Stat,
	1: syscall.Lstat,
}

func check_path_or_print_usage() bool {
	if len(os.Args) >= 2 {
		if _, err := os.Stat(os.Args[1]); os.IsNotExist(err) {
			fmt.Printf("Path %s doesn't exist\n", os.Args[1])
		} else {
			return true
		}
	} else {
		fmt.Printf("Usage: ./ls path [depth] \n")

	}

	return false
}

func gidToGrpStuct(gid uint32) (*user.Group, error) {
	grp, err := user.LookupGroupId(strconv.FormatUint(uint64(gid), 10))
	if err != nil {
		fmt.Println(err)
	}
	return grp, err
}

func uidToUserStuct(uid uint32) (*user.User, error) {
	usr, err := user.LookupId(strconv.FormatUint(uint64(uid), 10))
	if err != nil {
		fmt.Println(err)
	}
	return usr, err
}

func buildTypePermStr(file_mode uint32) (string, bool) {
	// Type and permissions string (tps)
	tps := ""
	lnk := false
	var mask uint32 = 0x0100

	switch file_mode & syscall.S_IFMT {
	case syscall.S_IFIFO:
		tps += "p"
	case syscall.S_IFCHR:
		tps += "c"
	case syscall.S_IFDIR:
		tps += "d"
	case syscall.S_IFBLK:
		tps += "b"
	case syscall.S_IFREG:
		tps += "-"
	case syscall.S_IFLNK:
		tps += "l"
		lnk = true
	case syscall.S_IFSOCK:
		tps += "s"
	default:
		fmt.Printf("ERROR INVALID MASK FOUND: %d\n", file_mode&syscall.S_IFMT)
		return tps, lnk
	}

	for i := 0; i < 9; i++ {
		switch file_mode & (mask >> i) {
		case syscall.S_IRUSR:
			tps += "r"
		case syscall.S_IWUSR:
			tps += "w"
		case syscall.S_IXUSR:
			tps += "x"
		case syscall.S_IRGRP:
			tps += "r"
		case syscall.S_IWGRP:
			tps += "w"
		case syscall.S_IXGRP:
			tps += "x"
		case syscall.S_IROTH:
			tps += "r"
		case syscall.S_IWOTH:
			tps += "w"
		case syscall.S_IXOTH:
			tps += "x"
		default:
			tps += "-"
		}
	}

	return tps, lnk
}

// Correct Go's sorting
func SortFileNameAscend(files []os.FileInfo) {
	sort.Slice(files, func(i, j int) bool {
		return strings.ToLower(files[i].Name()) < strings.ToLower(files[j].Name())
	})
}

func SortFileNameDescend(files []os.FileInfo) {
	sort.Slice(files, func(i, j int) bool {
		return strings.ToLower(files[i].Name()) > strings.ToLower(files[j].Name())
	})
}

func lsReadDir(root string) ([]os.FileInfo, error) {
	f, err := os.Open(root)
	if err != nil {
		return nil, err
	}
	fInfoList, err := f.Readdir(-1)
	f.Close()
	if err != nil {
		return nil, err
	}

	return fInfoList, nil
}

func recursive_walk_and_list(path string, currd uint, mdepth uint, opts uint32) {

	dirlist, err := lsReadDir(path)
	if err != nil {
		fmt.Printf("ioutil.ReadDir(%s) failed\n", path)
		fmt.Println(err)
		return
	}

	SortFileNameAscend(dirlist)

	for _, file := range dirlist {

		var fstat = syscall.Stat_t{}

		fpath := filepath.Join(path, file.Name())

		// smap is a map containing stat (key == 0) and
		// lstat (key == 1). This provides a fast way
		// to resolve the option to follow or not follow
		// symlinks (LS_PHYS)
		if err := smap[opts&LS_PHYS](fpath, &fstat); err != nil {

			if errno, ok := err.(syscall.Errno); ok {

				if errno == syscall.EACCES {
					fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
				} else {
					fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
				}
				continue
			}
		} else {
			lnkDst := ""
			grp, _ := gidToGrpStuct(fstat.Gid)
			usr, _ := uidToUserStuct(fstat.Uid)
			mode, lnk := buildTypePermStr(uint32(fstat.Mode))
			if lnk {
				tmp, _ := os.Readlink(fpath)
				lnkDst += " -> " + tmp
			}

			fmt.Printf(
				"%s %3d %4s %4s %10d %15s %-s\n",
				mode,
				fstat.Nlink,
				grp.Name,
				usr.Name,
				fstat.Size,
				time.Unix(fstat.Mtim.Unix()).Format(time.RFC3339),
				fpath+lnkDst,
			)
		}
		if (fstat.Mode&syscall.S_IFDIR) != 0 && currd != mdepth {
			recursive_walk_and_list(fpath, currd+1, mdepth, opts)
		}
	}
}

func main() {
	if !check_path_or_print_usage() {
		return
	}

	abspath, err := filepath.Abs(os.Args[1])
	if err != nil {
		fmt.Printf("filepath.Abs(%s) failed\n", os.Args[1])
		fmt.Println(err)
		return
	}

	var depth = 1
	if len(os.Args) >= 3 {
		depth, err = strconv.Atoi(os.Args[2])
		if err != nil {
			fmt.Printf("failed to convert %s to int", os.Args[2])
		}
	}
	recursive_walk_and_list(abspath, 1, uint(depth), 1)
}
