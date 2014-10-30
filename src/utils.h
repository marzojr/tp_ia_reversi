#ifndef _UTILS_H_
#define _UTILS_H_

// Templated swap function -- Requires assignable type
template<typename T1, typename T2> inline void coswap(T1 &a1, T1 &b1, T2 &a2, T2 &b2){
    T1 temp1(a1);
    a1 = b1;
    b1 = temp1;
    T2 temp2(a2);
    a2 = b2;
    b2 = temp2;
}

// More generic quicksort function (that allow's co-value sorting), if used with lambda-functions, for example
template<typename T1, typename T2> inline void quicksort(
    T1 * data,
    T2 * codata,
    size_t cnt,                                   // The total number of elements in the supposed array
    size_t startIdx = 0                           // The index at which to start sorting from
){
    // Return if sorting is done
    if (cnt < 2) return;
 
    // Otherwise, partition array by pivot
    size_t leftIdx = startIdx;
    size_t rightIdx = startIdx + cnt - 1;
    size_t pivotIdx = (leftIdx + rightIdx) / 2;
 
    // Copy the pivot onto the right, to ensure it doesn't swap during the procedure
    coswap(data[pivotIdx], data[rightIdx], codata[pivotIdx], codata[rightIdx]);
    pivotIdx = rightIdx;
    rightIdx--;
 
    // Do partition
    while (leftIdx != rightIdx){
 
        // If the left element is where it should be, leave it there and go on to the next one
        if (data[leftIdx] > data[pivotIdx]) leftIdx++;
 
        /* If the left element should be on the right of the pivot, swap it with the element on the right,
        * leave it to be checked next iteration, but move the right-side pointer so that element stays fixed */
        else {
            coswap(data[leftIdx], data[rightIdx], codata[leftIdx], codata[rightIdx]);
            rightIdx--;
        }
    }
 
    // Move leftIdx so it's necessarily over an element of the right, and then swap with pivot
    if (data[leftIdx] > data[pivotIdx]) leftIdx++;
    coswap(data[leftIdx], data[pivotIdx], codata[leftIdx], codata[pivotIdx]);
 
    // Recursively call quicksort, sorting everything but the pivot
    size_t q1Start = startIdx;
    size_t q2Start = leftIdx + 1;
    size_t q1Cnt = leftIdx - q1Start;
    size_t q2Cnt = cnt - q1Cnt - 1;
    quicksort(data, codata, q1Cnt, q1Start);
    quicksort(data, codata, q2Cnt, q2Start);
}

#endif

