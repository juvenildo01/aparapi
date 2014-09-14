package hsailtest;

import com.amd.aparapi.Device;
import org.junit.Test;

import static com.amd.aparapi.HSA.barrier;
import static com.amd.aparapi.HSA.getWorkGroupId;
import static com.amd.aparapi.HSA.getWorkItemId;
import static com.amd.aparapi.HSA.localIntX1;
import static org.junit.Assert.assertTrue;

public class ScanReductionMatchedStringsX2JUnit{

   int add(int lhs, int rhs){
      return (lhs+rhs);
   }

   void scan(int[] arr, int interval, int size){
      Device.hsa().forEach(arr.length/interval, id -> {
         int[] local = localIntX1(); // we need a local int buffer the same width as the group
         int lid = getWorkItemId();
         // int lsize = getCurrentWorkGroupSize();
         // Step 1 - copy global memory to local
         local[lid] = arr[((id+1)*interval)-1];
         barrier();
         // Step 2 - create intermediate sum by stepping in powers of 2 (1,2,4,8)
         for (int step = 2; step<=size; step *= 2){
            if ((lid+1)%step == 0){
               int stride = step/2;
               local[lid] += local[lid-stride];
            }
            barrier();
         }
         //  4    2    3    2    6    1    2    3
         //   \   |     \   |     \   |     \   |
         //    \  |      \  |      \  |      \  |
         //     \ |       \ |       \ |       \ |
         //      \|        \|        \|        \|
         //       +         +         +         +
         //  4    6    3    5    6    7    2    5
         //        \        |          \        |
         //         \       |           \       |
         //          \      |            \      |
         //           \     |             \     |
         //            \    |              \    |
         //             \   |               \   |
         //              \  |                \  |
         //               \ |                 \ |
         //                \|                  \|
         //                 +                   +
         //  4    6    3   11    6    7    2   12
         //                  \                  |
         //                   \                 |
         //                    \                |
         //                     \               |
         //                      \              |
         //                       \             |
         //                        \            |
         //                         \           |
         //                          \          |
         //                           \         |
         //                            \        |
         //                             \       |
         //                              \      |
         //                               \     |
         //                                \    |
         //                                 \   |
         //                                  \  |
         //                                   \ |
         //                                    \|
         //                                     +
         //  4    6    3   11    6    7    2   23
         // step 3 - add intermediate values by stepping down in powers of 2 (8,4,2,1)
         for (int step = size/2; step>1; step /= 2){
            if (((lid+1)<size) && (((lid+1)%step) == 0)){
               int stride = step/2;
               local[lid+stride] = local[lid]+local[lid+stride];
            }
            barrier();
         }
         //  4    6    3   11    6    7    2   23
         //                  \        |
         //                   \       |
         //                    \      |
         //                     \     |
         //                      \    |
         //                       \   |
         //                        \  |
         //                         \ |
         //                          \|
         //                           +
         //  4    6    3   11    6   18    2   23
         //        \   |     \   |     \   |
         //         \  |      \  |      \  |
         //          \ |       \ |       \ |
         //           \|        \|        \|
         //            +         +         +
         //  4    6   10   11   17   18   20   23
         // step 4 - copy local memory back to global memory
         arr[((id+1)*interval)-1] = local[lid];
         barrier();
      });
   }

   // Imagine we had 16 numbers          1 0 1 0     1 1 0 0     0 0 0 1     1 0 0 1
   // the result scan is                 1 1 2 2     3 4 4 4     4 4 4 5     6 6 6 7
   //
   // Here is how it works in parallel
   //
   // In 4 goups of 4                  [ 1 0 1 0 ] [ 1 1 0 0 ] [ 0 0 0 1 ] [ 1 0 0 1 ]
   //                                    |\| |\|     |\| |\|     |\| |\|     |\| |\|
   //                                    | + | +     | + | +     | + | +     | + | +
   //                                    | |\| |     | |\| |     | |\| |     | |\| |
   //                                    | | \ |     | | \ |     | | \ |     | | \ |
   //                                    | | |\|     | | |\|     | | |\|     | | |\|
   //                                    | | | +     | | | +     | | | +     | | | +
   //                                    | |\+ |     | |\+ |     | |\| |     | |\+ |
   // after the scan of each group     [ 1 1 2 2 ] [ 1 2 2 2 ] [ 0 0 0 1 ] [ 1 1 1 2 ]
   //                                          |\          |           |\          |
   //                                          |  \        |           |  \        |
   //                                          |    \      |           |    \      |
   //                                          |      \    |           |      \    |
   //                                          |        \  |           |        \  |
   //                                          |          \|           |          \|
   //                                          |           +           |           +
   //                                          |           | \         |           |
   //                                          |           |   \       |           |
   //                                          |           |     \     |           |
   //                                          |           |       \   |           |
   //                                          |           |         \ |           |
   //                                          |           |           | \         |
   //                                          |           |           |   \       |
   //                                          |           |           |     \     |
   //                                          |           |           |        \  |
   //                                          |           |           |          \|
   //                                          |           |           |           +
   //                                          |           +           |           +
   //                                          |           | \         |           |
   //                                          |           |   \       |           |
   //                                          |           |     \     |           |
   //                                          |           |       \   |           |
   //                                          |           |         \ |           |
   //                                          |           |           +           |
   //                                          |           |           |           |
   //                                          |           |           |           |
   //                                          |           |           |           |
   //                                          |           |           |           |
   //                                          |           |           |           |
   // after the scan across last item  [ 1 1 2 2 ] [ 1 2 2 4 ] [ 0 0 0 5 ] [ 1 1 1 7 ]
   // of each group                            |\    | | | |\    | | | |\    | | | |
   //                                          | \   | | | | \   | | | | \   | | | |
   //                                          |  ---+-+-+-+  ---+-+-+-+  ---+-+-+-+
   // after adding last element of     [ 1 1 2 2 ] [ 3 4 4 4 ] [ 4 4 4 5 ] [ 6 6 6 7 ]
   // prev group to all items in
   // subsequent

   void fixup(int[] arr, int interval, int size){
      Device.hsa().forEach(arr.length, id -> {
         int[] local = localIntX1();
         int lid = getWorkItemId();
         local[lid] = arr[id*interval];
         barrier();
         if (id>=size && lid<size-1){
            int partial = arr[(getWorkGroupId()*size)-1];
            local[lid] = local[lid]+partial;
         }
         barrier();
         arr[id] = local[lid];
         barrier();
      });
   }

   void filter(String[] strings, int[] arr){
      Device.hsa().forEach(arr.length, id -> {
         String s = strings[id];
         int len = s.length();
         if (len>2 && s.charAt(0) == s.charAt(len-1) && s.charAt(1) == s.charAt(len-2)){
            arr[id] = 1;
         }else{
            arr[id] = 0;
         }
      });
   }

   void map(String[] instrings, int[] arr, String[] outStrings){
      Device.hsa().forEach(arr.length, id -> {
         if (arr[id]>arr[id-1]){
            outStrings[arr[id]] = instrings[id];
         }
      });
   }

   @Test
   public void test(){
      final int len = 65536*4; // don't change this !
      String[] inStrings = new String[len];
      int[] in = new int[len];
      Device.jtp().forEach(len, id -> inStrings[id] = ""+id);
      filter(inStrings, in);
      int[] inCopy = JUnitHelper.copy(in);
      scan(in, 1, 256);
      scan(in, 256, 256);
      scan(in, 65536, 4);
      fixup(in, 256, 4);
      fixup(in, 1, 256);
      int outCount = in[len-1];
      JUnitHelper.nl("number of strings="+outCount);
      String[] outStrings = new String[outCount];
      map(inStrings, in, outStrings);
      for (String s : outStrings){
         JUnitHelper.nl(s);
      }
      int hsaSum = in[len-1];
      JUnitHelper.nl("hsaSum "+hsaSum);
      JUnitHelper.out("rule");
      for (int i = 0; i<1024; i++){
         if ((i%256) == 0){
            JUnitHelper.out(" V  ");
         }else{
            JUnitHelper.out(" .  ");
         }
      }
      JUnitHelper.nl();
      JUnitHelper.dump("orig", inCopy, "%3d", 1024);
      JUnitHelper.dump(" hsa", in, "%3d", 1024);
      int[] out = new int[len];
      out[0] = inCopy[0];
      for (int i = 1; i<len; i++){
         out[i] = out[i-1]+inCopy[i];
      }
      int sum = out[len-1]+inCopy[len-1];
      JUnitHelper.nl("sum "+sum);
      JUnitHelper.dump(" seq", out, "%3d", 1024);
      assertTrue("HSA equals JTP results", JUnitHelper.compare(out, in));
   }
}