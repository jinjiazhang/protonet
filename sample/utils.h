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

bool check_color(char chesses[], int row1, int col1, int row2, int col2) {
    int index1 = (row1 - 1) * 15 + (col1 - 1);
    int index2 = (row2 - 1) * 15 + (col2 - 1);
    if (chesses[index1] == 0 || chesses[index2] == 0) {
        return false;
    }
    return chesses[index1] % 2 == chesses[index2] % 2;
}

bool check_single(char chesses[], int row, int col, int diff_row, int diff_col) {
    int link_num = 1;
    for (int i = 1; i < 5; i++) {
        int tmp_row = row - diff_row * i;
        int tmp_col = col - diff_col * i;
        if (tmp_row <= 0 || tmp_row > 15 || tmp_col <= 0 || tmp_col > 15) {
            break;
        }
        if (!check_color(chesses, row, col, tmp_row, tmp_col)) {
            break; 
        }
        link_num += 1;
    }

    for (int i = 1; i < 5; i++) {
        int tmp_row = row + diff_row * i;
        int tmp_col = row + diff_col * i;
        if (tmp_row <= 0 || tmp_row > 15 || tmp_col <= 0 || tmp_col > 15) {
            break;
        }
        if (!check_color(chesses, row, col, tmp_row, tmp_col)) {
            break;
        }
        link_num += 1;
    }
    return link_num >= 5;
}

bool check_result(char chesses[], int row, int col) {
    if (check_single(chesses, row, col, 0, 1))
        return true;
    if (check_single(chesses, row, col, 1, 0))
        return true;
    if (check_single(chesses, row, col, 1, 1))
        return true;
    if (check_single(chesses, row, col, -1, 1))
        return true;
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