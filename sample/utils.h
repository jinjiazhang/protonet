#pragma once

bool check_valid(char chesses[], int row, int col) {
    if (row <= 0 || row > 15 || col <= 0 || col > 15) {
        return false;
    }

    int index = (row - 1) * 15 + (col - 1);
    if (chesses[index] != 0) {
        return false;
    }

    return true;
}

bool check_result(char chesses[]) {
    return false;
}

void draw_chesses(char chesses[]) {
    for (int row = 1; row <= 15; row++) {
        for (int col = 1; col <= 15; col++) {
            int index = (row - 1) * 15 + (col - 1);
            int value = chesses[index];
            if (value == 0) {
                printf("╂-");
            }
            else if (value % 2 == 0) {
                printf("●");
            } 
            else {
                printf("○");
            }
        }
        printf("\n");
    }
}