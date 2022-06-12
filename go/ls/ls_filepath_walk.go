package main

import (
	"fmt"
	"os"
	"path/filepath"
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

func walk_cb(path string, info os.FileInfo, err error) error {
	if err != nil {
		return err
	}
	println(path)
	return nil
}

func main() {

	if !check_path_or_print_usage() {
		return
	}

	filepath.Walk(os.Args[1], walk_cb)

}
