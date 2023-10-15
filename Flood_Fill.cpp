class Solution {
public:
    void fill(vector<vector<int>>& image, int sr, int sc, int color,int paint,int r,int c) {
        if(sr<0 || sr>=r || sc<0 || sc>=c){
            return;
        }
        if(image[sr][sc]!=color){
            return;
        }
        image[sr][sc]=paint;
           fill(image,sr-1,sc,color,paint,r,c);
            fill(image,sr,sc-1,color,paint,r,c);
            fill(image,sr,sc+1,color,paint,r,c);
            fill(image,sr+1,sc,color,paint,r,c);
        
        return;
    }

    vector<vector<int>> floodFill(vector<vector<int>>& image, int sr, int sc, int color) {
        if(image[sr][sc]!=color){
        fill(image,sr,sc,image[sr][sc],color,image.size(),image[0].size());
        }
        return image;
    }
};
