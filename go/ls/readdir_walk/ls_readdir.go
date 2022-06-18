package main

import (
	"fmt"
	"io/ioutil" // filesystem errors
	"os"
	"os/user"
	"path/filepath" // join
	"strconv"
	"syscall"
	"time"
)

const (
	MMMDDYYYYhhmm = "Jan 01 2022"
)

func check_path_or_print_usage() bool {
	if len(os.Args) >= 2 {
		if _, err := os.Stat(os.Args[1]); os.IsNotExist(err) {
			fmt.Printf("Path %s doesn't exist\n", os.Args[1])
		} else {
			return true
		}
	} else {
		fmt.Printf("Usage: ./ls [path]\n")
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

func lsTimeStr(time time.Time) string {
	return fmt.Sprintf("%s", time.Format("MMM DD YYYY HH:SS"))
}

var g_permMap = map[int]string{
	syscall.S_IRUSR: "r",
	syscall.S_IWUSR: "w",
	syscall.S_IXUSR: "x",
	syscall.S_IRGRP: "r",
	syscall.S_IWGRP: "w",
	syscall.S_IXGRP: "x",
	syscall.S_IROTH: "r",
	syscall.S_IWOTH: "w",
	syscall.S_IXOTH: "x",
}

/* Keys to efficiently iterate the g_permMap
 * Since these are known are build time
 * and order matter and will not change
 * Placing these here
 */
var g_keys = [9]uint32{
	uint32(syscall.S_IRUSR),
	uint32(syscall.S_IWUSR),
	uint32(syscall.S_IXUSR),
	uint32(syscall.S_IRGRP),
	uint32(syscall.S_IWGRP),
	uint32(syscall.S_IXGRP),
	uint32(syscall.S_IROTH),
	uint32(syscall.S_IWOTH),
	uint32(syscall.S_IXOTH),
}

func buildTypePermStr(file_mode uint32) string {
	// Type and permissions string (tps)
	tps := ""
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
	case syscall.S_IFSOCK:
		tps += "s"
	default:
		fmt.Printf("ERROR INVALID MASK FOUND: %d\n", file_mode&syscall.S_IFMT)
		return tps
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

	return tps
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

	dirlist, err := ioutil.ReadDir(abspath)
	if err != nil {
		fmt.Printf("ioutil.ReadDir(%s) failed\n", abspath)
		fmt.Println(err)
		return
	}

	for _, file := range dirlist {

		var fstat = syscall.Stat_t{}

		fpath := filepath.Join(abspath, file.Name())

		if err := syscall.Stat(fpath, &fstat); err != nil {

			if errno, ok := err.(syscall.Errno); ok {

				if errno == syscall.EACCES {
					fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
				} else {
					fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
				}
				continue
			}
		} else {
			grp, _ := gidToGrpStuct(fstat.Gid)
			usr, _ := uidToUserStuct(fstat.Uid)
			mode := buildTypePermStr(uint32(fstat.Mode))

			//fstat.Mtim.
			fmt.Printf(
				"%s %3d %4s %4s %10d %15s %-s\n",
				mode,
				fstat.Nlink,
				grp.Name,
				usr.Name,
				fstat.Size,
				"0",
				fpath,
			)
		}

	}

}
