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

		fpath := filepath.Join(abspath, file.Name())

		if fi, err := os.Stat(fpath); err != nil {

			if errno, ok := err.(syscall.Errno); ok {

				if errno == syscall.EACCES {
					fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
				} else {
					fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
				}
				continue
			}
		} else {
			fstat := fi.Sys().(*syscall.Stat_t)
			grp, _ := gidToGrpStuct(fstat.Gid)
			usr, _ := uidToUserStuct(fstat.Uid)
			fmt.Printf(
				"%s %3d %4s %4s %10d %15s %-s\n",
				fi.Mode(),
				fstat.Nlink,
				grp.Name,
				usr.Name,
				fstat.Size,
				fi.ModTime().Format("Jan 01"),
				fpath,
			)
		}

	}

}
