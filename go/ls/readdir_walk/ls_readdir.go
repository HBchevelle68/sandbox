package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"syscall"
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

func main() {

	if !check_path_or_print_usage() {
		return
	}

	dirlist, err := ioutil.ReadDir(os.Args[1])
	if err != nil {
		fmt.Printf("ioutil.ReadDir(%s) failed\n", os.Args[1])
		return
	}

	for _, file := range dirlist {

		fpath := filepath.Join(os.Args[1], file.Name())
		fmt.Println(fpath)

		if fstat, err := os.Stat(fpath); err != nil {

			if errno, ok := err.(syscall.Errno); ok {

				if errno == syscall.EACCES {
					fmt.Printf("Stat on %s failed... Permission Denied\n", fpath)
				} else {
					fmt.Printf("Stat on %s failed... errno val: %d Msg:%s\n", fpath, errno, err.Error())
				}

				continue
			}
		} else {
			fmt.Printf("fstat: %v\n", fstat.Name())
		}

	}

}
