public class ArrayReversal {
    public static void main(String[] args) {
        int[] arr = {1, 2, 3, 4, 5};
        reverseArray(arr);
        System.out.println("Reversed array: " + java.util.Arrays.toString(arr));
    }

    public static void reverseArray(int[] arr) {
        /*
         * Reverses the given array in-place by swapping elements.
         *
         * Parameters:
         * arr (int[]): The array to be reversed.
         */
        int start = 0;
        int end = arr.length - 1;

        while (start < end) {
            // Swap the elements at the start and end positions
            int temp = arr[start];
            arr[start] = arr[end];
            arr[end] = temp;

            start++;
            end--;
        }
    }
}
