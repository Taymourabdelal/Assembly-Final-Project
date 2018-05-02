package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(time.Now().Unix())
	winFlag := false
	sudokus := [9][9]int{}  //grid solutions
	sudokun := [9][9]int{}  //grid numbers visible
	sudokub := [9][9]bool{} //grid truth values
	initials(&sudokus, &sudokub)
	fmt.Println("initialization complete")
	fmt.Println()
	fmt.Println("Completed Sudoku: ")
	draw(&sudokus, &sudokub)
	sudokun = sudokus
	empty(&sudokus, &sudokun, &sudokub)
	fmt.Print(sudokub)
	fmt.Println("Playable Sudoku: ")
	draw(&sudokun, &sudokub)
	for winFlag == false {
		sudokuInput(&sudokun, &sudokub, &sudokus)
		draw(&sudokun, &sudokub)
		winFlag = winCondition(&sudokub)
	}
	fmt.Scanln()

}

func empty(grids *[9][9]int, gridn *[9][9]int, gridb *[9][9]bool) int { //empty random squares
	x := 0
	counter := 0
	flag := true
	for flag {
		for i := 0; i < 9; i++ {
			for j := 0; j < 9; j++ {
				x = rand.Intn(4)
				if x == 0 && gridb[i][j] && counter < 30 {
					gridb[i][j] = false
					gridn[i][j] = 0
					counter++
				}
			}
		}
		if counter == 30 {
			flag = false
		}
	}
	return 0
}

func draw(Grid *[9][9]int, Gridb *[9][9]bool) int { //draws the grid
	for i := 0; i < 9; i++ {
		for j := 0; j < 9; j++ {
			if j%3 == 0 && j > 0 {
				fmt.Print(" | ")
			}
			if Gridb[i][j] {
				fmt.Print(Grid[i][j], " ")
			} else {
				fmt.Print("_ ")
			}
		}
		if i%3 == 2 && i > 0 && i < 8 {
			fmt.Println()
			for k := 0; k < 23; k++ {
				fmt.Print("-")
			}
		}
		fmt.Println()
	}
	return 0
}

func initials(Sudokus *[9][9]int, Sudokub *[9][9]bool) int { //generate
	counter := 0

	for i := 0; i < 9; i++ {
		for j := 0; j < 9; j++ {
			if !Sudokub[i][j] {
				flag1 := false
				Sudokub[i][j] = true
				for !flag1 {
					y := 0
					x := rand.Intn(9) + 1

					for k := 0; k < 9; k++ { //check row
						if Sudokus[i][k] == x {
							y++
						}
					}
					for k := 0; k < 9; k++ { //check column
						if Sudokus[k][j] == x {
							y++
						}
					}
					for k := 3 * (i / 3); k < (3*(i/3) + 3); k++ { //check grid
						for l := 3 * (j / 3); l < (3*(j/3) + 3); l++ {
							if Sudokus[k][l] == x {
								y++
							}
						}
					}
					if y == 0 {
						Sudokus[i][j] = x
						flag1 = true
					}
					counter++
					if counter > 100 {
						counter = 0
						flag1 = true
						j = -1
						for z := 0; z < 9; z++ {
							Sudokus[i][z] = 0
							Sudokub[i][z] = false
						}
					}
				}
			}
		}
	}
	return 0
}

func sudokuInput(sudokun *[9][9]int, sudokub *[9][9]bool, sudokus *[9][9]int) {
	var x int
	var y int
	var input int

	fmt.Print("Enter the X value : ")
	fmt.Scanln(&x)
	fmt.Print(x)
	fmt.Print("Enter the Y value : ")
	fmt.Scanln(&y)

	fmt.Print("Enter the number: ")
	fmt.Scanln(&input)
	fmt.Print(sudokub[x][y])
	if sudokub[x][y] == false {
		sudokun[x][y] = input
		fmt.Print(sudokun[x][y])
		if sudokun[x][y] == sudokus[x][y] {
			sudokub[x][y] = true
		}
	} else {
		fmt.Print("This is not a valid location")
	}

}

func winCondition(sudokub *[9][9]bool) bool {

	flag := true
	for i := 0; i < 9; i++ {
		for j := 0; j < 9; j++ {

			if sudokub[i][j] == false {

				flag = false

				return flag
			}

		}
	}
	return true
}
