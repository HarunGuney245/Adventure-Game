#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_ROOMS 10
#define MAX_INVENTORY 5

// Struct Definitions
typedef struct {
    char name[50];
    int health;
    int strength;
    char inventory[MAX_INVENTORY][50];
    int inventoryCount;
} Player;

typedef struct {
    char description[200];
    char items[MAX_INVENTORY][50];
    int itemCount;
    int hasCreature;
    int creatureHealth;
    int creatureStrength;
    int connections[4]; // up, down, left, right
} Room;

// Global Variables
Player player;
Room rooms[MAX_ROOMS];
int currentRoom = 0;

// Function Prototypes
void initializeGame();
void displayPrompt();
void processCommand(char *command);
void move(char *direction);
void look();
void inventory();
void pickup(char *item);
void attack();
void listSavedGames();
void saveGame(char *filepath);
void loadGame(char *filepath);
void exitGame();

int main() {
    initializeGame();
    printf("Welcome to the Adventure Game!\n");

    while (1) {
        displayPrompt();
        char command[100];
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;  // Remove newline
        processCommand(command);
    }
    return 0;
}

// Initialize the game with sample data
void initializeGame() {
    strcpy(player.name, "Hero");
    player.health = 100;
    player.strength = 10;
    player.inventoryCount = 0;

    // Room 0
    strcpy(rooms[0].description, "You are in a dark, musty room.");
    rooms[0].itemCount = 1;
    strcpy(rooms[0].items[0], "Sword");
    rooms[0].hasCreature = 1;
    rooms[0].creatureHealth = 30;
    rooms[0].creatureStrength = 5;
    rooms[0].connections[0] = -1; // No upward connection
    rooms[0].connections[1] = 1;  // Connects down to room 1

    // Room 1
    strcpy(rooms[1].description, "You are in a forest clearing. The trees are tall, and the sound of birds fills the air.");
    rooms[1].itemCount = 1;
    strcpy(rooms[1].items[0], "Shield");
    rooms[1].hasCreature = 1;
    rooms[1].creatureHealth = 40;
    rooms[1].creatureStrength = 7;
    rooms[1].connections[0] = 0; // Connects up to room 0
    rooms[1].connections[1] = 2; // Connects down to room 2

    // Room 2
    strcpy(rooms[2].description, "You find yourself in a mysterious cave with glistening crystals on the walls.");
    rooms[2].itemCount = 1;
    strcpy(rooms[2].items[0], "Healing Potion");
    rooms[2].hasCreature = 1;
    rooms[2].creatureHealth = 50;
    rooms[2].creatureStrength = 10;
    rooms[2].connections[0] = 1; // Connects up to room 1
    rooms[2].connections[1] = 3; // Connects down to room 3

    // Room 3
    strcpy(rooms[3].description, "You are in an ancient library filled with dusty books and a faint smell of parchment.");
    rooms[3].itemCount = 1;
    strcpy(rooms[3].items[0], "Magic Scroll");
    rooms[3].hasCreature = 1;
    rooms[3].creatureHealth = 60;
    rooms[3].creatureStrength = 12;
    rooms[3].connections[0] = 2; // Connects up to room 2
    rooms[3].connections[1] = -1; // No downward connection
}

// Display game prompt
void displayPrompt() {
    printf("%s> ", player.name);
}

// Command Processing
void processCommand(char *command) {
    if (strncmp(command, "move", 4) == 0) {
        move(command + 5);
    } else if (strcmp(command, "look") == 0) {
        look();
    } else if (strcmp(command, "inventory") == 0) {
        inventory();
    } else if (strncmp(command, "pickup", 6) == 0) {
        pickup(command + 7);
    } else if (strcmp(command, "attack") == 0) {
        attack();
    } else if (strcmp(command, "list") == 0) {
        listSavedGames();
    } else if (strncmp(command, "save", 4) == 0) {
        saveGame(command + 5);
    } else if (strncmp(command, "load", 4) == 0) {
        loadGame(command + 5);
    } else if (strcmp(command, "exit") == 0) {
        exitGame();
    } else {
        printf("Unknown command. Try again.\n");
    }
}

// Movement Handler
void move(char *direction) {
    int dirIndex = -1;
    if (strcmp(direction, "up") == 0) dirIndex = 0;
    else if (strcmp(direction, "down") == 0) dirIndex = 1;
    else if (strcmp(direction, "left") == 0) dirIndex = 2;
    else if (strcmp(direction, "right") == 0) dirIndex = 3;

    if (dirIndex != -1 && rooms[currentRoom].connections[dirIndex] != -1) {
        currentRoom = rooms[currentRoom].connections[dirIndex];
        printf("You moved %s.\n", direction);
        look();
    } else {
        printf("You can't move that way.\n");
    }
}

// Look Command

void look() {
	int i;
    printf("%s\n", rooms[currentRoom].description);
    if (rooms[currentRoom].itemCount > 0) {
        printf("You see: ");
        for (i = 0; i < rooms[currentRoom].itemCount; i++) {
            printf("%s ", rooms[currentRoom].items[i]);
        }
        printf("\n");
    }
    if (rooms[currentRoom].hasCreature) {
        printf("A creature is here!\n");
    }
}

// Inventory Command
void inventory() {
	int i;
    printf("Inventory:\n");
    for (i = 0; i < player.inventoryCount; i++) {
        printf("- %s\n", player.inventory[i]);
    }
}

// Pickup Command
void pickup(char *item) {
	int i,j;
    if (player.inventoryCount >= MAX_INVENTORY) {
        printf("Your inventory is full!\n");
        return;
    }

    for (i = 0; i < rooms[currentRoom].itemCount; i++) {
        if (strcmp(rooms[currentRoom].items[i], item) == 0) {
            strcpy(player.inventory[player.inventoryCount++], item);
            printf("You picked up: %s\n", item);
            for ( j = i; j < rooms[currentRoom].itemCount - 1; j++) {
                strcpy(rooms[currentRoom].items[j], rooms[currentRoom].items[j + 1]);
            }
            rooms[currentRoom].itemCount--;
            return;
        }
    }
    printf("There is no %s here.\n", item);
}
void attack() {
    if (!rooms[currentRoom].hasCreature) {
        printf("There is nothing to attack here.\n");
        return;
    }

    while (player.health > 0 && rooms[currentRoom].creatureHealth > 0) {
        rooms[currentRoom].creatureHealth -= player.strength;
        printf("You attacked the creature! Its health is now %d.\n", rooms[currentRoom].creatureHealth);

        if (rooms[currentRoom].creatureHealth <= 0) {
            printf("You defeated the creature!\n");
            rooms[currentRoom].hasCreature = 0;
            return;
        }

        player.health -= rooms[currentRoom].creatureStrength;
        printf("The creature attacked you! Your health is now %d.\n", player.health);
    }

    if (player.health <= 0) {
        printf("You were defeated. Game over.\n");
        exit(0);
    }
}

void listSavedGames() {
    struct dirent *entry;
    DIR *dp = opendir(".");

    if (dp == NULL) {
        printf("Unable to list saved games.\n");
        return;
    }

    printf("Saved Games:\n");
    while ((entry = readdir(dp))) {
        if (strstr(entry->d_name, ".txt")) {
            printf("- %s\n", entry->d_name);
        }
    }
    closedir(dp);
}

void saveGame(char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (!file) {
        printf("Failed to save the game.\n");
        return;
    }
    fwrite(&player, sizeof(Player), 1, file);
    fwrite(&rooms, sizeof(Room), MAX_ROOMS, file);
    fwrite(&currentRoom, sizeof(int), 1, file);
    fclose(file);
    printf("Game saved successfully to %s.\n", filepath);
}

void loadGame(char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("Failed to load the game.\n");
        return;
    }
    fread(&player, sizeof(Player), 1, file);
    fread(&rooms, sizeof(Room), MAX_ROOMS, file);
    fread(&currentRoom, sizeof(int), 1, file);
    fclose(file);
    printf("Game loaded successfully from %s.\n", filepath);
}

void exitGame() {
    printf("Exiting the game. Goodbye!\n");
    exit(0);
}



