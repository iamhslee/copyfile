# 2024-1 Operating Systems (ITP30002) - HW #1
# File  : Makefile
# Author: Hyunseo Lee (22100600) <hslee@handong.ac.kr>

CC=gcc
SOURCE=main.c
TARGET=copyfile

all:
	@echo "Build $(TARGET)..."
	$(CC) $(SOURCE) -o $(TARGET)
	@echo "Build $(TARGET) done."

debug:
	@echo "Build $(TARGET) with debug mode..."
	$(CC) $(SOURCE) -Wall -Wextra -DDEBUG -o $(TARGET)
	@echo "Build $(TARGET) with debug mode done."

testenv:
	@make clean
	@echo "Creating test directory..."
	@mkdir TEST
	@mkdir TEST/DIR_A
	@mkdir TEST/DIR_B
	@mkdir TEST/DIR_C
	@mkdir TEST/DIR_A/SUBDIR_A
	@mkdir TEST/DIR_A/SUBDIR_B
	@mkdir TEST/DIR_A/SUBDIR_C
	@touch TEST/test1.txt
	@touch TEST/test2.txt
	@touch TEST/test3.txt
	@touch TEST/DIR_A/1.txt
	@touch TEST/DIR_A/2.txt
	@touch TEST/DIR_A/3.txt
	@touch TEST/DIR_B/4.txt
	@touch TEST/DIR_B/5.txt
	@echo "ultrices mi tempus imperdiet nulla malesuada pellentesque elit eget gravida" > TEST/test1.txt
	@echo "mauris a diam maecenas sed enim ut sem viverra aliquet" > TEST/test2.txt
	@echo "viverra aliquet eget sit amet tellus cras adipiscing enim eu" > TEST/test3.txt
	@echo "viverra suspendisse potenti nullam ac tortor vitae purus faucibus ornare" > TEST/DIR_A/1.txt
	@echo "id velit ut tortor pretium viverra suspendisse potenti nullam ac" > TEST/DIR_A/2.txt
	@echo "venenatis urna cursus eget nunc scelerisque viverra mauris in aliquam" > TEST/DIR_A/3.txt
	@echo "imperdiet proin fermentum leo vel orci porta non pulvinar neque" > TEST/DIR_B/4.txt
	@echo "maecenas pharetra convallis posuere morbi leo urna molestie at elementum" > TEST/DIR_B/5.txt
	@echo "Test directory created."

clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET)
	@rm -rf $(TARGET).dSYM
	@rm -rf TEST
	@echo "Cleaned up."