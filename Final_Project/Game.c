#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



//Initialize structs
typedef struct {
	char name[50];
	int score;
} Player;

//Function declarations
void display_instructions();
void start_game(char *name, int score, int lives);
char *generate_item(int *correct);

void save_game(char *name, int score, int lives);
void load_game(char *savefilename);

void load_scores(char *filename);
void add_score(char *name, int score);
void sort_scores(Player *leaderboard, int low, int high);
void save_scores(char *filename);
void show_scores();

//Global variables
int allocated = 0;
Player *leaderboard = NULL;
char *savefilename = NULL;

//Run the game through main
int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Enter a save file\n");
		exit(1);
	}

	//Seed for random num function
	srand(time(NULL));

	printf("Welcome to Recycle Run!\n");

	FILE *file = fopen(argv[1], "a+");
	if (file == NULL) {
		printf("Error opening save file\n");
		exit(1);
	}

	//Checks if the save file is empty or not
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fclose(file);

	savefilename = argv[1];

	Player currPlayer;
	printf("Enter your name: ");	
	scanf("%s", currPlayer.name);
	currPlayer.score = 0;

	load_scores("leaderboard.txt");

	int choice;
	while (1) {
		printf("\nMenu\n");
		printf("(0) Start new game\n(1) Load game\n(2) Display leaderboard\n(3) Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);

		switch (choice) {
			case 0:
				display_instructions();
				start_game(currPlayer.name, 0, 2);
				save_scores("leaderboard.txt");
				break;
			case 1:
				if (fileSize == 0) {
					printf("No save available\n");
					break;
				}
				load_game(argv[1]);	
				save_scores("leaderboard.txt");
				break;
			case 2:
				show_scores();
				break;
			case 3:
				free(leaderboard);
				exit(0);
			default:
				printf("Invalid input\n");
		}
	}
}

//Loads the inputted save file
void load_game(char *savefilename) {
	FILE *file = fopen(savefilename, "r");
	if (file == NULL) {
		printf("Error opening save file");
		free(leaderboard);
		exit(1);
	}

	char name[50];
	int score, lives;
	fscanf(file, "%s %d %d", name, &score, &lives);
	fclose(file);

	printf("Game progress loaded sucessfully.\n");
	start_game(name, score, lives);
}

//Saves the game to the save file
void save_game(char *name, int score, int lives) {
	FILE *file = fopen(savefilename, "w");
	if (file == NULL) {
		printf("Error opening save file\n");
		free(leaderboard);
		exit(1);
	}
	fseek(file, 0, SEEK_SET);
	fprintf(file, "%s %d %d", name, score, lives);
	fclose(file);
	
	printf("Game progress saved sucessfully.\n");
}

//Displays the instructions of the game
void display_instructions() {
	printf("\nHow to play:\n");
	printf("- Sort items into the correct recycling bin.\n");
	printf("- Bins: 1 = Plastic | 2 = Paper | 3 = Electronics | 4 = Glass\n");
	printf("- Correct answers earn you 10 points!\n");
	printf("- You get 2 incorrect answers and the game ends.\n");
	printf("- You can save your progress at any point\n");
}

//Generates a random item for the game
char *generate_item(int *correct) {
	int correct_bin = rand() % 4 + 1;
	int randomnum2 = rand() % 10;
	char *item;

	char *plastic[] = {
                                "Water bottle",
                                "Detergent bottle",
                                "Straw",
                                "Toothbrush",
                                "Nylon fishing net",
                                "Tupperware",
                                "Grocery bag",
                                "Ziplock bag",
                                "Shampoo bottle",
                                "Clear fork"
                        };

	char *paper[] = {
                                "Book",
                                "Cardboard box",
                                "Newspaper",
                                "Envelope",
                                "Business card",
                                "Napkin",
                                "Magazine",
                                "Cereal box",
                                "Phone book",
                                "Milk carton"
                        };

	char *electronic[] = {
                                "Computer moniter",
                                "Printer",
                                "DVD player",
                                "Tablet",
                                "Electric toothbrush",
                                "Camera",
                                "Cell Phone",
                                "Xbox",
                                "iPod",
                                "Laptop"
                        };

	 char *glass[] = {
                                "Wine bottle",
                                "Pickle jar",
                                "Lightbulb",
                                "Candle jar",
                                "Glass cup",
                                "Window",
                                "Monocle",
                                "Vase",
                                "Teacup",
                                "Flask"
                        };

	switch (correct_bin) {
		case 1:
			item = plastic[randomnum2];
			break;

		case 2:
			item = paper[randomnum2];
			break;

		case 3:
                        item = electronic[randomnum2];
			break;

		case 4:
                        item = glass[randomnum2];
			break;
	}
	*correct = correct_bin;
	return item;
}

//Starts the game
void start_game(char *name, int score, int lives) {
	do {
		int correct;
		int action;
		printf("Your current score: %d | Lives: %d\n", score, lives);
		printf("You stumble upon a %s\n", generate_item(&correct));
		printf("Put it into the correct bin\n");
		printf("(1) Plastic\n(2) Paper\n(3) Electronics\n(4) Glass\n(5) Save Progress and exit\n");
		scanf("%d", &action);

		if (action >= 1 && action <= 5) {
			if (action == 5) {
				save_game(name, score, lives);
				exit(0);
			}
			else if (action == correct) {
				score += 10;
				printf("Correct! +10 points\n");
			}
			else {
				lives--;
				printf("Incorrect. -1 life\n");
			}
		}
		else {
			printf("Invalid choice. Try again.\n");
		}

		if (lives <= 0) {
			printf("\nGame Over! Final Score: %d\n", score);
			add_score(name, score);
			sort_scores(leaderboard, 0, allocated - 1);
			break;
		}
	}
	while (lives > 0);
	return;
}

//Loads the leaderboard for scores
void load_scores(char *filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening %s\n", filename);
		exit(1);
	}

	while (1) {
		leaderboard = realloc(leaderboard, (allocated + 1) * sizeof(Player));
		if (leaderboard == NULL) {
			printf("Error reallocating memory for loading\n");
			fclose(file);
			exit(1);
		}

		if (fscanf(file, "%s %d", leaderboard[allocated].name, &leaderboard[allocated].score) != 2) {
			break;
		}
		allocated++;
	}

	fclose(file);
	printf("\nLeaderboard loaded successfully!\n");
}

//Adds a score to the leaderboard
void add_score(char *name, int score) {
	strcpy(leaderboard[allocated].name, name);
	leaderboard[allocated].score = score;
	allocated++;
	printf("\nScore added to leaderboard.\n");
}

//Swaps two player elements
void swap(Player *a, Player *b) {
	Player temp = *a;
	*a = *b;
	*b = temp;
}

//Partition function to place the pivot element at the correct position
int partition(Player *leaderboard, int low, int high) {
    int pivot = leaderboard[high].score;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (leaderboard[j].score >= pivot) {
            i++;
            swap(&leaderboard[i], &leaderboard[j]);
        }
    }
    swap(&leaderboard[i + 1], &leaderboard[high]);
    return (i + 1);
}

//Uses the quicksort algorithm to sort the scores from highest to lowest
void sort_scores(Player *leaderboard, int low, int high) {
	if (low < high) {
		int pi = partition(leaderboard, low, high);
		sort_scores(leaderboard, low, pi - 1);
		sort_scores(leaderboard, pi + 1, high);
	}
}

//Saves the scores to the leaderboard file
void save_scores(char *filename) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		printf("Error opening %s\n", filename);
		exit(1);
	}

	for (int i = 0; i < allocated; i++) {
		fprintf(file, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
	}

	fclose(file);
	printf("Leaderboard updated and saved successfully!\n");
}

//Displays the leaderboard
void show_scores() {
	if (allocated == 0) {
		printf("Leaderboard is empty or not available.\n");
		return;
	}
	else {
		printf("%-20s %-10s\n", "Name", "Score");
		printf("------------------------------\n");
		for (int i = 0; i< allocated && i < 10; i++) {
			printf("%-20s %-10d\n", leaderboard[i].name, leaderboard[i].score);
		}
		printf("------------------------------\n");
	}
}	
