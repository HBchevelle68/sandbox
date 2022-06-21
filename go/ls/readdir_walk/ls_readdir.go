package main

import (
	"flag"
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

// Covert uid to user.Group
func gidToGrpStuct(gid uint32) (*user.Group, error) {
	grp, err := user.LookupGroupId(strconv.FormatUint(uint64(gid), 10))
	if err != nil {
		fmt.Println(err)
	}
	return grp, err
}

// Covert uid to user.User
func uidToUserStuct(uid uint32) (*user.User, error) {
	usr, err := user.LookupId(strconv.FormatUint(uint64(uid), 10))
	if err != nil {
		fmt.Println(err)
	}
	return usr, err
}

// go offers a way to acces this already produced string
// however, i found multiple cases where it would evaluate
// The type to have multiple types. I ran into this simply
// by running on /. This while manual produces identical
// linux output
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

// There are func's in go that can produce similar output
// However, all i tried seemed to run stat() instead of
// lstat(). This is problematic when you don't want to
// eval the link destination but what the link itself
//
// This function gets around this by using os.File.Readdir()
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

func processFile(fpath string, opts uint32, fstat *syscall.Stat_t) {
	// smap is a map containing stat (key == 0) and
	// lstat (key == 1). This provides a fast way
	// to resolve the option to follow or not follow
	// symlinks (LS_PHYS)
	if err := smap[opts&LS_PHYS](fpath, fstat); err != nil {

		if errno, ok := err.(syscall.Errno); ok {

			if errno == syscall.EACCES {
				fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
			} else {
				fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
			}
			return
		}
	} else {
		// If anything errors here
		// We just push on and display what we can
		lnkDst := ""
		grp, _ := gidToGrpStuct(fstat.Gid)
		usr, _ := uidToUserStuct(fstat.Uid)
		mode, lnk := buildTypePermStr(uint32(fstat.Mode))
		if lnk {
			tmp, _ := os.Readlink(fpath)
			lnkDst += " -> " + tmp
		}

		fmt.Printf(
			"%s %3d %8s %8s %10d %15s %-s\n",
			mode,
			fstat.Nlink,
			grp.Name,
			usr.Name,
			fstat.Size,
			time.Unix(fstat.Mtim.Unix()).Format(time.RFC3339),
			fpath+lnkDst,
		)
	}
}

func recursive_walk_and_list(path string, rootDev uint64, currd uint, mdepth uint, opts uint32) {

	dirlist, err := lsReadDir(path)
	if err != nil {
		fmt.Println(err)
		return
	}
	// The sorting in go isn't as expected
	// i.e. doesn't match the sorting performed
	// by Linux utils, this will fix that
	SortFileNameAscend(dirlist)

	for _, file := range dirlist {

		var fstat = syscall.Stat_t{}
		fpath := filepath.Join(path, file.Name())

		if ((opts&LS_NODIR) != 0 && file.IsDir()) || ((opts&LS_ONLYDIR) != 0 && !file.IsDir()) {
			// One of the options we were told to skip has hit
			// Skip full processing, just stat the file
			// We need to Xstat() in order to obtain
			// the device ID the file is on
			if err := smap[opts&LS_PHYS](fpath, &fstat); err != nil {

				if errno, ok := err.(syscall.Errno); ok {

					if errno == syscall.EACCES {
						fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
					} else {
						fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
					}
					return
				}
			}
		} else {
			// Process AND print the file
			processFile(fpath, opts, &fstat)
		}

		// Recurse
		// I dislike how long this conditional is
		// but stacking in go causes weird indentation
		// the conditional itself...
		if (fstat.Mode&syscall.S_IFDIR) != 0 && currd != mdepth && (fstat.Dev == rootDev && (opts&LS_MOUNT) != 0) {
			recursive_walk_and_list(fpath, rootDev, currd+1, mdepth, opts)
		}
	}
}

func main() {

	// Set flags
	userPath := flag.String("p", ".", "Path")
	depth := flag.Int("d", 1, "Max levels deep to interate. 1 is current directory. -1 is unlimited.")
	sym := flag.Bool("follow-sym", false, "Follow symlinks")
	xDev := flag.Bool("x", true, "If set, stay within the same filesystem (i.e., do not cross mount points).")
	noDir := flag.Bool("no-dir", false, "Filter out directories. Directories will STILL be traversed.")
	onlyDir := flag.Bool("only-dir", false, "Filter out everything that is NOT a directory")
	flag.Parse()

	if *noDir && *onlyDir {
		fmt.Println("Cannot select no directories and only directories")
		flag.Usage()
		return
	}

	abspath, err := filepath.Abs(*userPath)
	if err != nil {
		fmt.Printf("filepath.Abs(%s) failed\n", *userPath)
		fmt.Println(err)
		return
	}

	var fstat = syscall.Stat_t{}
	if err := syscall.Lstat(abspath, &fstat); err != nil {

		if errno, ok := err.(syscall.Errno); ok {

			if errno == syscall.EACCES {
				fmt.Printf("Stat on %s failed... Permission Denied\n", abspath)
			} else {
				fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", abspath, errno, err.Error())
			}
			return
		}
	}

	var optmask uint32 = 0
	if !(*sym) {
		optmask |= LS_PHYS
	}
	if *xDev {
		optmask |= LS_MOUNT
	}
	if *noDir {
		optmask |= LS_NODIR
	}
	if *onlyDir {
		optmask |= LS_ONLYDIR
	}

	recursive_walk_and_list(abspath, fstat.Dev, 1, uint(*depth), optmask)
}
