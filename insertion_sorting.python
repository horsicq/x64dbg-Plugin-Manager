def insertion_sort(arr):
    """
    Sorts a list of comparable elements using the Insertion Sort algorithm.

    Args:
        arr (list): List of elements to be sorted in-place.

    Returns:
        None (the list is sorted in-place).
    """
    for i in range(1, len(arr)):
        # Store the current element to be inserted into the sorted part of the list.
        current_element = arr[i]
        j = i - 1
        
        # Move elements of arr[0..i-1] that are greater than current_element one position ahead of their current position.
        while j >= 0 and current_element < arr[j]:
            arr[j + 1] = arr[j]
            j -= 1
        
        # Insert the current_element at its correct position in the sorted part of the list.
        arr[j + 1] = current_element

# Example usage:
if __name__ == "__main__":
    unsorted_list = [12, 11, 13, 5, 6]
    print("Unsorted List:", unsorted_list)
    insertion_sort(unsorted_list)
    print("Sorted List:", unsorted_list)
