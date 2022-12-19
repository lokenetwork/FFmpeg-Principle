package MyLib;
public class MyMath {
    public int sum(int[] args) {
        int all = 0;
        for(int num : args){
            all += num;
        }
        return all;
    }

}