//İsmet Gökay 21050111025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INVENTORY 5
#define MAX_HEALTH 100
#define TOTAL_ROOMS 5

typedef struct Creature {
    char *name;
    int health;
    int strength;
} Creature;

typedef struct {
    char map[7][7];
    Creature *creatures[3];
    int creature_positions[3][2];
    int creature_count;
} Room;

typedef struct {
    char name[50];
    int health;
    int strength;
    int armor_level;
    int inventory[MAX_INVENTORY];
    int inventory_count;
} Player;

void initialize_map(Room *room, int room_number);
void print_map(Room *room);
void battle(Player *player, Creature *creature);
Creature* generate_creature();
Creature* find_creature(Room *room, int x, int y);
void use_inventory(Player *player);
void look(Room *room);
void save_game(Player *player, Room *room, int player_x, int player_y, int current_room);
void load_game(Player *player, Room *room, int *player_x, int *player_y, int *current_room);
int allCreaturesDefeated(Room *room);

int main() {
    srand(time(NULL));

    Room room;
    Player player = {"Hero", MAX_HEALTH, 20, 0, {0}, 0};

    int player_x = 6, player_y = 4; 
    int current_room = 0;       
    FILE *save_file = fopen("save_game.dat", "rb");
    if (save_file != NULL) {
        printf("A saved game exists. Do you want to load it? (yes/no): ");
        char choice[10];
        scanf("%s", choice);
        if (strcmp(choice, "yes") == 0) {
            fread(&player, sizeof(Player), 1, save_file);
            fread(&room, sizeof(Room), 1, save_file);
            fread(&player_x, sizeof(int), 1, save_file);
            fread(&player_y, sizeof(int), 1, save_file);
            fread(&current_room, sizeof(int), 1, save_file);
            fclose(save_file);
            printf("Game loaded successfully!\n");
        } else {
            fclose(save_file);
            initialize_map(&room, current_room);
            room.map[player_x][player_y] = 'P';
        }
    } else {
        initialize_map(&room, current_room);
        room.map[player_x][player_y] = 'P';
    }
    while (1) {
        print_map(&room);
        printf("\nPlayer Health: %d | Strength: %d | Armor Level: %d\n", 
               player.health, player.strength, player.armor_level);
        printf("Inventory: ");
        if (player.inventory_count == 0) {
            printf("Empty\n");
        } else {
            for (int i = 0; i < player.inventory_count; i++) {
                printf("Potion ");
            }
            printf("\n");
        }
        printf("Move (up, down, left, right, inventory, look, exit): ");
        char command[10];
        scanf("%s", command);

        int new_x = player_x;
        int new_y = player_y;
        int reset_position = 1;

        if (strcmp(command, "up") == 0) {
            new_x--;
        } else if (strcmp(command, "down") == 0) {
            new_x++;
        } else if (strcmp(command, "left") == 0) {
            new_y--;
        } else if (strcmp(command, "right") == 0) {
            new_y++;
        } else if (strcmp(command, "inventory") == 0) {
            printf("\nInventory:\n");
            if (player.inventory_count == 0) {
                printf("Your inventory is empty.\n");
            } else {
                printf("Potions: %d\n", player.inventory_count);
                use_inventory(&player);
            }
            continue;
        } else if (strcmp(command, "look") == 0) {
            look(&room);
            continue;
        } else if (strcmp(command, "save") == 0) {
            FILE *save_file = fopen("save_game.dat", "wb");
            if (save_file != NULL) {
                fwrite(&player, sizeof(Player), 1, save_file);
                fwrite(&room, sizeof(Room), 1, save_file);
                fwrite(&player_x, sizeof(int), 1, save_file);
                fwrite(&player_y, sizeof(int), 1, save_file);
                fwrite(&current_room, sizeof(int), 1, save_file);
                fclose(save_file);
                printf("Game saved successfully!\n");
            } else {
                printf("Failed to save the game.\n");
            }
            continue;
        }else if (strcmp(command, "exit") == 0) {
            printf("Exiting the game...\n");
            break;
        } else {
            printf("Invalid command! Please try again.\n");
            continue;
        }
        if (new_x >= 0 && new_x < 7 && new_y >= 0 && new_y < 7 && room.map[new_x][new_y] != '1') {
            if (room.map[new_x][new_y] == 'C') {
                Creature *creature = find_creature(&room, new_x, new_y);
                if (creature != NULL) {
                    printf("\nYou encountered a creature: %s! Do you want to attack? (Type 'attack' to fight or anything else to avoid): ", creature->name);
                    char choice[10];
                    scanf("%s", choice);

                    if (strcmp(choice, "attack") == 0) {
                        battle(&player, creature);
                        if (player.health <= 0) {
                            printf("You died! Game over.\n");
                            return 0;
                        }
                        if (creature->health <= 0) {
                            printf("You defeated %s!\n", creature->name);
                            room.map[new_x][new_y] = '0';
                        } else {
                            continue;
                        }
                    } else {
                        printf("You chose to avoid the creature.\n");
                        continue;
                    }
                }
            }

            if (room.map[new_x][new_y] == 'H') {
                printf("\nYou found a potion! Do you want to pick it up? (Type 'pick up' to take it, or anything else to leave it): ");
                char choice[10];
                scanf("%s", choice);
                if (strcmp(choice, "pick") == 0) {
                    scanf("%s", choice);
                    if (strcmp(choice, "up") == 0 && player.inventory_count < MAX_INVENTORY) {
                        printf("\nYou picked up a potion! It is added to your inventory.\n");
                        player.inventory[player.inventory_count++] = 1;
                        room.map[new_x][new_y] = '0'; 
                    } else {
                        printf("\nYou decided not to take the potion or your inventory is full.\n");
                        continue;
                    }
                } else {
                    printf("\nYou decided not to take the potion.\n");
                    continue;
                }
            } else if (room.map[new_x][new_y] == 'S') {
                printf("\nYou found a sword! Do you want to pick it up? (Type 'pick up' to take it, or anything else to leave it): ");
                char choice[10];
                scanf("%s", choice);
                if (strcmp(choice, "pick") == 0) {
                    scanf("%s", choice);
                    if (strcmp(choice, "up") == 0) {
                        printf("\nYou picked up the sword! Your strength has increased.\n");
                        player.strength += 10;
                        room.map[new_x][new_y] = '0';
                    } else {
                        printf("\nYou decided not to take the sword.\n");
                        continue;
                    }
                } else {
                    printf("\nYou decided not to take the sword.\n");
                    continue;
                }
            } else if (room.map[new_x][new_y] == 'A') {
                printf("\nYou found armor! Do you want to pick it up? (Type 'pick up' to take it, or anything else to leave it): ");
                char choice[10];
                scanf("%s", choice);
                if (strcmp(choice, "pick") == 0) {
                    scanf("%s", choice);
                    if (strcmp(choice, "up") == 0) {
                        player.armor_level++;
                        printf("\nYou picked up the armor! Your armor level is now %d. You take less damage from creatures.\n", player.armor_level);
                        room.map[new_x][new_y] = '0';
                    } else {
                        printf("\nYou decided not to take the armor.\n");
                        continue;
                    }
                } else {
                    printf("\nYou decided not to take the armor.\n");
                    continue;
                }
            }

            if (room.map[new_x][new_y] == 'D') {
              if(allCreaturesDefeated(&room) == 1){  
                if (current_room < TOTAL_ROOMS - 1) {
                    printf("\nYou found a door! Moving to the next room...\n");
                    current_room++;
                    initialize_map(&room, current_room);
                    player_x = 6;
                    player_y = 4;
                    room.map[player_x][player_y] = 'P';
                    continue;
                } else {
                    printf("\nCongratulations! You have completed the game!\n");
                    break;
                }
             }else{
                printf("Door is locked please defeat all creature");
                continue;
             }
            }

            room.map[player_x][player_y] = '0'; 
            player_x = new_x;
            player_y = new_y;
            room.map[player_x][player_y] = 'P'; 
        } else {
            printf("You cannot move in that direction!\n");
        }
    }

    return 0;
}
void initialize_map(Room *room, int room_number) {
    char maps[TOTAL_ROOMS][7][7] = {
        {{'1', '1', '1', '1', 'D', '1', '1'},
         {'1', 'H', '0', '0', 'C', '0', '1'},
         {'1', 'C', '1', '0', '1', '0', '1'},
         {'1', '0', '0', 'C', '0', '0', '1'},
         {'1', '0', '1', '0', '1', '0', '1'},
         {'1', '0', '0', '0', '0', '0', '1'},
         {'1', '1', '1', '1', 'P', '1', '1'}},

        {{'1', 'D', '1', '1', '1', '1', '1'},
         {'1', '0', 'C', '0', 'C', 'S', '1'},
         {'1', '0', '1', '0', '1', '0', '1'},
         {'1', '0', '1', '0', '1', 'H', '1'},
         {'1', '0', '1', 'C', '1', '0', '1'},
         {'1', 'A', '1', '0', '1', '0', '1'},
         {'1', '1', '1', '1', 'P', '1', '1'}},

         {{'1', '1', '1', '1', '1', '1', '1'},
         {'D', '0', '0', 'C', '0', '0', '1'},
         {'1', '1', '1', '1', '1', '0', '1'},
         {'1', 'C', '0', '0', 'H', '0', '1'},
         {'1', '0', '1', '1', '1', '1', '1'},
         {'1', '0', '0', 'C', '0', '0', '1'},
         {'1', '1', '1', '1', 'P', '1', '1'}},

         {{'1', '1', '1', '1', '1', '1', '1'},
         {'1', 'S', '0', 'C', '0', 'A', '1'},
         {'1', '0', '1', 'H', '1', '0', '1'},
         {'1', 'C', '1', 'D', '1', 'C', '1'},
         {'1', '0', '1', '1', '1', '0', '1'},
         {'1', '0', '0', '0', '0', '0', '1'},
         {'1', '1', '1', '1', 'P', '1', '1'}},

         {{'1', 'D', '1', '1', '1', '1', '1'},
         {'1', 'C', '1', '0', '1', '1', '1'},
         {'1', '0', '1', '0', '1', '1', '1'},
         {'1', '0', '0', 'C', '0', '0', '1'},
         {'1', '1', '1', '1', '0', '0', '1'},
         {'1', 'H', '0', 'C', '0', '0', '1'},
         {'1', '1', '1', '1', 'P', '1', '1'}}};

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            room->map[i][j] = maps[room_number][i][j];
        }
    }
    room->creature_count = 0;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (room->map[i][j] == 'C') {
                if (room->creature_count < 3) {
                    room->creatures[room->creature_count] = generate_creature();
                    room->creature_positions[room->creature_count][0] = i;
                    room->creature_positions[room->creature_count][1] = j;
                    room->creatures[room->creature_count]->health += room_number * 10;
                    room->creatures[room->creature_count]->strength += room_number * 2; 
                    room->creature_count++;
                }
            }
        }
    }
}
void print_map(Room *room) {
    printf("\nMap:\n");
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            printf("%c ", room->map[i][j]);
        }
        printf("\n");
    }
}

Creature* generate_creature() {
    Creature *creature = malloc(sizeof(Creature));
    char *names[] = {"Creature"};
    int rand_index = rand() % 1;
    creature->name = names[rand_index];
    creature->health = 30 + rand() % 21;
    creature->strength = 5 + rand() % 11;
    return creature;
}


Creature* find_creature(Room *room, int x, int y) {
    for (int i = 0; i < room->creature_count; i++) {
        if (room->creature_positions[i][0] == x && room->creature_positions[i][1] == y) {
            return room->creatures[i];
        }
    }
    return NULL;
}

int allCreaturesDefeated(Room *room) {
     for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++)  {
            if (room->map[i][j] == 'C') {
                return 0;
            }
        }
    }
    return 1;
}

void battle(Player *player, Creature *creature) {
    printf("Battle started! Fighting %s...\n", creature->name);

    while (player->health > 0 && creature->health > 0) {
        creature->health -= player->strength;
         printf("%s attacks %s for %d damage!\n", player->name, creature->name, player->strength);
        if (creature->health <= 0) {
            printf("You defeated the creature!\n");
            return;
        }
        int damage = creature->strength;
        if (player->armor_level > 0) {
            damage = damage * (100 - 10 * player->armor_level) / 100;
            if (damage < 1) damage = 1;
        }

        player->health -= damage;
        printf("%s attacks %s for %d damage!\n", creature->name, player->name, damage);
    }

    if (player->health <= 0) {
        printf("You were defeated by the creature. Game over!\n");
    }
}

void use_inventory(Player *player) {
    if (player->inventory_count == 0) {
        printf("\nYour inventory is empty! Nothing to use.\n");
        return;
    }

    printf("\nUse the item by typing its name (e.g., 'potion'). Type 'exit' to leave the inventory.\n");
    char item_name[10];
    scanf("%s", item_name);

    if (strcmp(item_name, "potion") == 0 && player->inventory_count > 0) {
        printf("\nYou used a potion! Your health has increased by 20.\n");
        player->health += 20;

        if (player->health > MAX_HEALTH) {
            player->health = MAX_HEALTH;
        }

        player->inventory_count--;
    } else if (strcmp(item_name, "exit") == 0) {
        return;
    } else {
        printf("\nInvalid item or action!\n");
    }
}

void save_game(Player *player, Room *room, int player_x, int player_y, int current_room) {
    FILE *file = fopen("savegame.txt", "w");
    if (!file) {
        printf("Error saving the game!\n");
        return;
    }
    fprintf(file, "%s %d %d %d %d\n", player->name, player->health, player->strength, player->armor_level, current_room);
    fprintf(file, "%d ", player->inventory_count);
    for (int i = 0; i < player->inventory_count; i++) {
        fprintf(file, "%d ", player->inventory[i]);
    }
    fprintf(file, "\n");
    fprintf(file, "%d %d\n", player_x, player_y);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            fprintf(file, "%c", room->map[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Game saved successfully!\n");
}

void load_game(Player *player, Room *room, int *player_x, int *player_y, int *current_room) {
    FILE *file = fopen("savegame.txt", "r");
    if (!file) {
        printf("No saved game found!\n");
        return;
    }

    fscanf(file, "%s %d %d %d %d\n", player->name, &player->health, &player->strength, &player->armor_level, current_room);

    fscanf(file, "%d ", &player->inventory_count);
    for (int i = 0; i < player->inventory_count; i++) {
        fscanf(file, "%d ", &player->inventory[i]);
    }
    fscanf(file, "\n");

    fscanf(file, "%d %d\n", player_x, player_y);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            fscanf(file, " %c", &room->map[i][j]);
        }
        fscanf(file, "\n");
    }

    fclose(file);
    printf("Game loaded successfully!\n");
}


void look(Room *room) {
    printf("\nLooking around the room...\n");
    int found = 0;

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            if (room->map[i][j] == 'H') {
                printf("You see a potion at (%d, %d).\n", i, j);
                found = 1;
            } else if (room->map[i][j] == 'S') {
                printf("You see a sword at (%d, %d).\n", i, j);
                found = 1;
            } else if (room->map[i][j] == 'C') {
                Creature *creature = find_creature(room, i, j);
                if (creature != NULL) {
                    printf("You see a creature at (%d, %d).\n", i, j);
                    found = 1;
                }
            } else if (room->map[i][j] == 'D') {
                printf("You see a door at (%d, %d).\n", i, j);
                found = 1;
            }else if (room->map[i][j] == 'A') {
                printf("You see armor at (%d, %d).\n", i, j);
                found = 1;
}
        }
    }

    if (!found) {
        printf("The room is empty.\n");
    }
}
