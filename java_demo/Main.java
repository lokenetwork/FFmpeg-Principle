import MyLib.MyMath;
public class Main {
    public static void main(String[] args) {
        int[] myList = {1,2,3,5};
        int all = 0;
        MyMath ma = new MyMath();
        all = ma.sum(myList);
        System.out.println(all);
    }
}
