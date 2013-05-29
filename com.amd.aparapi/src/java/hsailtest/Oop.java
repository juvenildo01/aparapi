package hsailtest;

import com.amd.aparapi.AparapiException;
import com.amd.aparapi.ClassModel;
import com.amd.aparapi.ClassParseException;
import com.amd.aparapi.OkraRunner;
import com.amd.aparapi.RegISA;
import com.amd.aparapi.RegISARenderer;


public class Oop {

    public static class P{
        int x;
        int y;
        P(int _x, int _y){
           x = _x;
           y = _y;
        }
    }

   P[] points = new P[10];


   public void run(int id) {
       points[id].x= id;
       points[id].y= id;
   }

   public void test() throws ClassParseException{
      ClassModel classModel = ClassModel.getClassModel(Oop.class);
      ClassModel.ClassModelMethod method = classModel.getMethod("run", "(I)V");
      method.getInstructions();
      OkraRunner runner = new OkraRunner();

      RegISARenderer renderer = new RegISARenderer();
      renderer.setShowComments(true);
      new RegISA(method).render(renderer);
      System.out.println(renderer.toString());
       for (int i=0; i< points.length; i++){
           points[i]=new P(0,0);
       }

      runner.run(renderer.toString(), points.length, this,  points.length);
       for (int i=0; i< points.length; i++){
           System.out.print("("+points[i].x+","+points[i].y+"),");
       }
   }




   public static void main(String[] args) throws AparapiException{
      (new Oop()).test();

   }
}
