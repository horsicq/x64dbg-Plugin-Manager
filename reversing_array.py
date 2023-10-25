def reverse_array(arr):
    """
    Reverses the order of elements in an array.

    Parameters:
    arr (list): The array to be reversed.

    Returns:
    list: The reversed array.
    """
    # Use slicing to reverse the array
    reversed_arr = arr[::-1]
    return reversed_arr

# Example usage:
if __name__ == "__main__":
    original_array = [1, 2, 3, 4, 5]
    reversed_array = reverse_array(original_array)
    print("Original array:", original_array)
    print("Reversed array:", reversed_array)
