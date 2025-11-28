#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    int x, y;
    int room1_x, room1_y;
    int room2_x, room2_y;
} Wall;

typedef struct {
    int* parent;
    int* rank;
} DisjointSet;

DisjointSet* create_disjoint_set(int n) {
    DisjointSet* set = malloc(sizeof(DisjointSet));
    set->parent = malloc(n * sizeof(int));
    set->rank = malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        set->parent[i] = i;
        set->rank[i] = 0;
    }
    
    return set;
}

int find_set(DisjointSet* set, int x) {
    if (set->parent[x] != x) {
        set->parent[x] = find_set(set, set->parent[x]);
    }
    return set->parent[x];
}

void union_sets(DisjointSet* set, int x, int y) {
    int root_x = find_set(set, x);
    int root_y = find_set(set, y);
    
    if (root_x == root_y) return;
    
    if (set->rank[root_x] < set->rank[root_y]) {
        set->parent[root_x] = root_y;
    } else {
        set->parent[root_y] = root_x;
        if (set->rank[root_x] == set->rank[root_y]) {
            set->rank[root_x]++;
        }
    }
}

void free_disjoint_set(DisjointSet* set) {
    free(set->parent);
    free(set->rank);
    free(set);
}

int room_to_index(int x, int y, int width) {
    return y * width + x;
}

void shuffle_walls(Wall* walls, int count) {
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Wall temp = walls[i];
        walls[i] = walls[j];
        walls[j] = temp;
    }
}

void init_maze(char** maze, int maze_size) {
    int actual_size = maze_size * 2 + 1;
    
    for (int i = 0; i < actual_size; i++) {
        for (int j = 0; j < actual_size; j++) {
            maze[i][j] = '#';
        }
    }
    
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            maze[i * 2 + 1][j * 2 + 1] = '.';
        }
    }
}

int count_walls(int maze_size) {
    return 2 * maze_size * (maze_size - 1);
}

Wall* generate_walls(int maze_size, int* wall_count) {
    *wall_count = count_walls(maze_size);
    Wall* walls = malloc(*wall_count * sizeof(Wall));
    int wall_idx = 0;
    
    for (int y = 0; y < maze_size - 1; y++) {
        for (int x = 0; x < maze_size; x++) {
            walls[wall_idx].x = x * 2 + 1;
            walls[wall_idx].y = y * 2 + 2;
            walls[wall_idx].room1_x = x;
            walls[wall_idx].room1_y = y;
            walls[wall_idx].room2_x = x;
            walls[wall_idx].room2_y = y + 1;
            wall_idx++;
        }
    }
    
    for (int y = 0; y < maze_size; y++) {
        for (int x = 0; x < maze_size - 1; x++) {
            walls[wall_idx].x = x * 2 + 2;
            walls[wall_idx].y = y * 2 + 1;
            walls[wall_idx].room1_x = x;
            walls[wall_idx].room1_y = y;
            walls[wall_idx].room2_x = x + 1;
            walls[wall_idx].room2_y = y;
            wall_idx++;
        }
    }
    
    return walls;
}

void generate_maze(char** maze, int maze_size) {
    int wall_count;
    Wall* walls = generate_walls(maze_size, &wall_count);
    
    shuffle_walls(walls, wall_count);
    
    DisjointSet* set = create_disjoint_set(maze_size * maze_size);
    
    int min_walls_to_remove = maze_size * maze_size - 1;
    
    int max_walls_to_keep = wall_count - min_walls_to_remove;
    int min_walls_to_keep = max_walls_to_keep * 0.15;
    int walls_removed = 0;
    
    for (int i = 0; i < wall_count && walls_removed < min_walls_to_remove; i++) {
        int room1_idx = room_to_index(walls[i].room1_x, walls[i].room1_y, maze_size);
        int room2_idx = room_to_index(walls[i].room2_x, walls[i].room2_y, maze_size);
        
        if (find_set(set, room1_idx) != find_set(set, room2_idx)) {
            maze[walls[i].y][walls[i].x] = '.';
            
            union_sets(set, room1_idx, room2_idx);
            walls_removed++;
        }
    }
    
    free_disjoint_set(set);
    free(walls);
}

void print_maze(char** maze, int actual_size) {
    for (int i = 0; i < actual_size; i++) {
        for (int j = 0; j < actual_size; j++) {
            printf("%c", maze[i][j]);
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    
    int maze_size = 6;
    int actual_size = maze_size * 2 + 1;
    
    char** maze = malloc(actual_size * sizeof(char*));
    for (int i = 0; i < actual_size; i++) {
        maze[i] = malloc(actual_size * sizeof(char));
    }
    
    init_maze(maze, maze_size);
    generate_maze(maze, maze_size);
    print_maze(maze, actual_size);
    
    for (int i = 0; i < actual_size; i++) {
        free(maze[i]);
    }
    free(maze);
    
    return 0;
}
