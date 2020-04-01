#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

typedef struct linear_system{  
    double ** matrix;
    double * f;
    int n;
    int * index_arr;
    int leading_el;
    double ** I;
}linear_system;                                         //структура, описывающая все компаненты данных задач


void input(linear_system * X, FILE * fp, int task) {    //функция ввода данных из файла или из командной строки
    fscanf(fp, "%d", &(X->n));
    int N = X->n;
    X->matrix = malloc(N * sizeof(double *));
    if (task == 3 || task == 4) {
        X->I = malloc(N * sizeof(double *));
    }

    for (int i = 0; i < N; i++) {
        X->matrix[i] = malloc(N * sizeof(double));
        if (task == 3 || task == 4) {
            X->I[i] = malloc(N * sizeof(double *));
        }
        for (int j = 0; j < N; j++) {
            fscanf(fp, "%lf", &X->matrix[i][j]);
            if (task == 3 || task == 4) {
                X->I[i][j] = (i == j);
            }
        }
    }

    if (task == 1 || task == 5) {
        X->f = malloc(N * sizeof(double));
    }

    X->index_arr = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) {
        if (task == 1 || task == 5) {
            fscanf(fp, "%lf", &(X->f[i]));
        }
        X->index_arr[i] = i;
    }
    return;
}


void input_prel(linear_system * X, int task) {          //генерация матрицы из приложения 2
    int n = 20, m = 8;
    double n_d = 20.0, m_d = 8.0;
    X->n = n;
    X->matrix = malloc(n * sizeof(double *));
    if (task == 1 || task == 5) {
        X->f = malloc(n * sizeof(double));
    }
    if (task == 3 || task == 4) {
        X->I = malloc(n * sizeof(double *));
    }
    for (int i = 0;i < n; i++) {
        X->matrix[i] = malloc(n * sizeof(double));
        if (task >= 3) {
            X->I[i] = malloc(n * sizeof(double));
        }
    }
    X->index_arr = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        for (int j = 0;j < n; j++) {
            if (i == j) {
                X->matrix[i][j] = n_d + m_d*m_d + j / m_d + i / n_d;
            } else {
                X->matrix[i][j] = ((double)(i + j))/(n_d + m_d);
            }
            if (task >= 3) {
                X->I[i][j] = (i == j);
            }
            
        }
        if (task == 1) {
                X->f[i] = 200.0 + 5.0 * i;
        }
        X->index_arr[i] = i;
    }
}


FILE * input_selection(linear_system * X) {                     //ввод способа получения матрицы           
    char choice1;                                               //из файла (+путь до файла), из командной строки
    printf("Сгенерировать матрицу из приложения 2 [y/n]: ");    //или генерировать из приложения 2
    scanf("%c", &choice1);
    
    if(choice1 == 'y') {
        return NULL;
    }

    scanf("%c", &choice1);
    char choice2;
    printf("Считывать матрицу из файла? [y/n]: ");
    scanf("%c", &choice2);

    if (choice2 == 'y') {
        char file_path[PATH_MAX];
        printf("Путь до файла: ");
        scanf("%s", file_path);
        FILE * fp = fopen(file_path, "r");
        return fp;
    }
    return stdin;
}


int get_none_zero_row(linear_system * X, int iter, int task) {   //функция выбора строки с ненулевым ведущим элементом
    int ind_none_zero = 0;                                       //и последующая перестоновка этой строки с ведущей

    for(int i = iter; i < X->n; i++) {
        if (X->matrix[i][iter] != 0) {
            ind_none_zero = i;
            break;
        }
    }

    double * tmp = X->matrix[iter];
    X->matrix[iter] = X->matrix[ind_none_zero];
    X->matrix[ind_none_zero] = tmp;

    if (task == 3) {
        tmp = X->I[iter];
        X->I[iter] = X->I[ind_none_zero];
        X->I[ind_none_zero] = tmp;
    }

    if (task == 1) {
        double tmp2 = X->f[iter];
        X->f[iter] = X->f[ind_none_zero];
        X->f[ind_none_zero] = tmp2;
    }

    return !(iter == ind_none_zero);
}


void show_matrix(linear_system * X, int task) {                 //вывод матрицы в командную строку
    printf("-----------------\n");
    for (int i = 0; i < X->n; i++) {
        for (int j = 0;j < X->n; j++) {
            printf("%0.2lf ", X->matrix[i][j]);
        }
        if (task == 1 || task == 5) {
            printf("| %0.2lf", X->f[i]);
        } else if (task == 3) {
            printf("| ");
            for (int j = 0;j < X->n; j++) {
                printf("%0.2lf ", X->I[i][j]);
            }
        }
        printf("\n");
    }
    return;
}


int chose_leading_element(linear_system * X, int iter) {      //функция для поиска ведущего элемента строки
    double max = fabs(X->matrix[iter][iter]);                 //и последующего переставления его столбца с ведущим
    int max_ind = iter;
    for (int i = iter + 1;i < X->n; i++) {
        if (fabs(X->matrix[iter][i]) > max) {
            max = fabs(X->matrix[iter][i]);
            max_ind = i;
        }
    }
    for (int i = 0; i < X->n; i++) {
        double tmp = X->matrix[i][iter];
        X->matrix[i][iter] = X->matrix[i][max_ind];
        X->matrix[i][max_ind] = tmp;
    }
    int tmp = X->index_arr[iter];
    X->index_arr[iter] = X->index_arr[max_ind];
    X->index_arr[max_ind] = tmp;
    return !(max_ind == iter);
}


double straight_run(linear_system * X, int task) {            //прямой ход в алгоритме Гаусса
    double det = 1.0;                                         //приведение к верхней треугольной матрицы с 1 на диагонали
    for (int i = 0; i < X->n; i++) {
        // printf("\niter: %d\n", i + 1);
        if (task == 3 || !X->leading_el) {
            if (get_none_zero_row(X, i, task)) {
                det = -det;
            }
        }
        // show_matrix(X);
        if (task != 3 && X->leading_el && chose_leading_element(X, i)) {
            det = -det;
        }
        // show_matrix(X);
        double leading_a = X->matrix[i][i];

        for(int j = i; j < X->n; j++) {
            X->matrix[i][j] /= leading_a;
        }

        if (task == 1) {
            X->f[i] /= leading_a;
        }

        if (task == 2) {
            det *= leading_a;
        }

        if (task == 3) {
            for (int j = 0; j < X->n; j++) {
                X->I[i][j] /= leading_a;
            }
        }

        for(int k = i + 1; k < X->n; k++) {
            double a = X->matrix[k][i];
            X->matrix[k][i] = 0;
            for(int j = i + 1; j < X->n; j++) {
                X->matrix[k][j] -= a * X->matrix[i][j];
            }
            if (task == 1) {
                X->f[k] -= a * X->f[i];
            }
            if (task == 3) {
                for (int j = 0; j < X->n; j++) {
                    X->I[k][j] -= a * X->I[i][j];
                }
            }
        }
        // show_matrix(X, task);
    }
    return det;
}


double * back_run(linear_system * X, int task) {            //обратный ход для решения СЛАУ и нахождения определителя
    double * ans = malloc(X->n * sizeof(double));           //методом Гаусса
    for (int i = X->n - 1; i >= 0; i--) {
        ans[i] = X->f[i];
        for (int j = i + 1; j < X->n; j++) {
            ans[i] -= X->matrix[i][j] * ans[j];
        }
    }
    // show_matrix(X, task);
    return ans;
}


void back_run_reverse(linear_system * X, int task) {       //обратный ход для нахождения обратной матрицы
    for (int i = X->n - 1; i >= 1; i--) {
        for (int j = 0; j < i; j++) {
            double a = X->matrix[j][i];
            for (int k = 0; k < X->n; k++) {
                X->I[j][k] -= a * X->I[i][k];
            }
            X->matrix[j][i] = 0;
            // show_matrix(X, task);
        }
    }
    return;
}


double MatrixNorm(double ** matrix, int n) {            //нахождение бесконечной нормы матрицы
    double max = 0;
    for (int i = 0; i < n; i++) {
        double sum = 0;
        for (int j = 0; j < n; j++) {
            sum += fabs(matrix[i][j]);
        }
        if (sum > max) {
            max = sum;
        }
    }
    return max;
}


double nevyazka(linear_system * X, double * ans) {     //функция подсчета невязки ( ||A(x_k) - f|| )
    double res = 0;
    for (int i = 0; i < X->n; i++) {
        double sum = 0;
        for (int j = 0;j < X->n; j++) {
            sum += X->matrix[i][j] * ans[j];
        }
        res += (sum - X->f[i]) * (sum - X->f[i]);
    }
    return sqrt(res);
}


double * Relax(linear_system * X, double eps, double om) {      //реализация алгоритма релаксации
    double * ans = calloc(X->n, sizeof(double));
    int iter = 0;
    while (nevyazka(X, ans) > eps) {
        for (int i = 0; i < X->n; i++) {
            double sum = 0;
            for (int j = 0; j < X->n; j++) {
                sum += X->matrix[i][j] * ans[j];
            }
            ans[i] = ans[i] + (om / X->matrix[i][i]) * (X->f[i] - sum);
        }
        iter++;
    }
    printf("Количество итераций: %d  ", iter);
    return ans;
}


int main(int argc, char *argv[]) {
    int task;
    linear_system * X = malloc(sizeof(linear_system));
    sscanf(argv[1], "%d", &task);
    X->leading_el = 0;
    if (task < 3 && argc > 2) {
        sscanf(argv[2], "%d", &(X->leading_el));
    }
    FILE * fp = input_selection(X);
    if (fp == NULL) {
        input_prel(X, task);
    } else {
        input(X, fp, task);
    }
    printf("\nИсходная матрица:\n");
    show_matrix(X, task);
    printf("\n");
    if (task == 1) {
        straight_run(X, task);
        double * ans = back_run(X, task);
        printf("Решение СЛАУ:\n");
        for(int j = 0;j < X->n; j++) {
            for (int i = 0; i < X->n; i++) {
                if (X->index_arr[i] == j) {
                    printf("x%d: %lf\n", j + 1, ans[i]);
                }
            }
        }
    } else if (task == 2) {
        printf("Определитель: %lf\n", straight_run(X, task));
    } else if (task == 3) {
        straight_run(X, task);
        // show_matrix(X, task);
        back_run_reverse(X, task);
        printf("A^(-1): \n");
        for (int i = 0; i < X->n; i++) {
            for (int j = 0; j < X->n; j++) {
                printf("%.2lf ", X->I[i][j]);
            }
            printf("\n");
        }
    } else if (task == 4) {
        double A_norm = MatrixNorm(X->matrix, X->n);
        straight_run(X, 3);
        back_run_reverse(X, 3);
        double A_inv_norm = MatrixNorm(X->I, X->n);
        printf("Степень обусловаленности матрицы: %lf\n", A_norm * A_inv_norm);
    } else if (task == 5) {
        double om = 0.1, eps = 0.0001;
        for (int i = 1; i < 20; i++) {
            printf("Омега: %lf ", om * i);
            double * ans = Relax(X, eps, om * i);
            for (int i = 0; i < X->n; i++) {
                printf("x%d: %lf  ", i+1, ans[i]);
            } 
            printf("\n");
        }
    }
    // show_matrix(X, task);
    for (int i = 0; i < X->n;i++) {
        free(X->matrix[i]);
        if (task == 3 || task == 4) {
            free(X->I[i]);
        }
    }
    free(X->matrix);
    if (task == 3 || task == 4) {
        free(X->I);
    }
    if (task == 1 || task == 5) {
        free(X->f);
    }
    if (task <=2){
        free(X->index_arr);
    }
    free(X);
    fclose(fp);
    return 0;
}