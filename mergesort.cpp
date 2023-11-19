#include <iostream>
#include <thread>
#include <future>

using namespace std;

void merge(int* arr, int left, int middle, int right);
void mergeSort(int* arr, int left, int right);

const int MAX_THREADS = thread::hardware_concurrency(); // максимальное количество потоков

void mergeSortParallel(int* arr, int left, int right) {
    if (left < right) {
        if (right - left <= 1000000) { // если размер массива меньше или равен 1000000, то будем сортировать последовательно
            mergeSort(arr, left, right);
        }
        else { // иначе делаем разделение массива и сортируем каждую часть параллельно
            int middle = (left + right) / 2;
            future<void> fut1 = async(launch::async, mergeSortParallel, arr, left, middle); // запуск первой половины в отдельном потоке
            future<void> fut2 = async(launch::async, mergeSortParallel, arr, middle + 1, right); // запуск второй половины в отдельном потоке
            fut1.wait(); // ожидание завершения первого потока
            fut2.wait(); // ожидание завершения второго потока
            merge(arr, left, middle, right); // слияние отсортированных частей
        }
    }
}

void merge(int* arr, int left, int middle, int right) {
    int n1 = middle - left + 1;
    int n2 = right - middle;

    int* L = new int[n1];
    int* R = new int[n2];

    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[middle + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

void mergeSort(int* arr, int left, int right) {
    if (left < right) {
        int middle = (left + right) / 2;
        mergeSort(arr, left, middle);
        mergeSort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

int main() {
    srand(0);
    long arr_size = 100000000;
    int* array = new int[arr_size];
    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 500000;
    }

    time_t start, end;

    // многопоточный запуск
    time(&start);
    mergeSortParallel(array, 0, arr_size - 1);
    time(&end);

    double seconds = difftime(end, start);
    printf("The time: %f seconds\n", seconds);

    for (long i = 0; i < arr_size - 1; i++) {
        if (array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }

    for (long i = 0; i < arr_size; i++) {
        array[i] = rand() % 500000;
    }

    // однопоточный запуск
    time(&start);
    mergeSort(array, 0, arr_size - 1);
    time(&end);

    seconds = difftime(end, start);
    printf("The time: %f seconds\n", seconds);

    delete[] array;
    return 0;
}