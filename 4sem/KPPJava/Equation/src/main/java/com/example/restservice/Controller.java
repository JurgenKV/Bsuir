package com.example.restservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.beans.factory.annotation.Autowired;
import org.thymeleaf.util.ListUtils;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.stream.Collectors;
import java.util.stream.Stream;

@RestController
public class Controller {
    @Autowired
    private final CacheMap cacheMap;
    // CacheMap map;
    //  @Autowired
    //Semaphor sema;

    public Controller(CacheMap cacheMap) {
        this.cacheMap = cacheMap;
    }

    // private List<Equation> myList = new ArrayList<Equation>();
    //  private static void Eq();
    @GetMapping("/equation")
    public List<Equation> eq(@RequestParam(value = "a", defaultValue = "1") List<String> a,
                             @RequestParam(value = "b", defaultValue = "2") List<String> b,
                             @RequestParam(value = "n", defaultValue = "1") List<String> n,
                             ArrayList<Equation> result) throws InterruptedException {
        // System.out.println(result);
        List<Equation> myList = new ArrayList<Equation>();
        List<Equation> resList = new ArrayList<Equation>();
        Semaphor.sem.release();
        //////////////////////////////////////// 500
    /*  Integer a1 = Integer.parseInt(a);
        Integer b1 = Integer.parseInt(b);
        Integer n1 = Integer.parseInt(n);
    */  ////////////////////////////////////////
        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        myList = ObjEqListFirst(a, b, n);

        //   if (!cacheMap.isStored(id)) {

          /*  if (isNumeric(a) && isNumeric(b) && isNumeric(n)) {
                LOGGER.log(Level.INFO, "Прошло валидацию =)");
                Integer a1 = Integer.parseInt(a);
                Integer b1 = Integer.parseInt(b);
                Integer n1 = Integer.parseInt(n);

                Equation equal = new Equation(a1, b1, n1);

                //      myList.add(equal);
                //    Thread.sleep(1000 * 3);
                //    cacheMap.insert(id, equal);
*/
     /*   myList.addAll(result.stream()
                .parallel()
                .forEach(ser ->)*/


        resList.addAll(myList.stream()
                .parallel()
                .filter(ser -> cacheMap.isStored(ser))
                .map(ser -> cacheMap.getKey(ser.getKeyEq()))
                .collect(Collectors.toList()));


        resList.addAll(myList.stream()
                .parallel()
                .filter(ser -> !cacheMap.isStored(ser))
                .map(ser -> {
                    cacheMap.insert(ser.getKeyEq(), ser);
                    return cacheMap.getKey(ser.getKeyEq());
                })
                .collect(Collectors.toList()));

        Comparator<Equation> comparatorMax = Comparator.comparing(Equation::getResult);
        Comparator<Equation> comparatorMin = Comparator.comparing(Equation::getResult);
        Comparator<Equation> comparatorAvg = Comparator.comparing(Equation::getKeyEq);

// Get Min or Max Object

        Equation minObject = resList.stream().min(comparatorMin).get();
        Equation maxObject = resList.stream().max(comparatorMax).get();

       // resList.stream().max(comparatorMax).get();


        System.out.println("maxObject = " + maxObject.result);
        System.out.println("minObject = " + minObject.result);


        return resList;
               /*  result.stream()
                .parallel()
                .filter(ser -> cacheMap.isStored(ser.getKeyEq()))
                .forEach(ser -> myList.add(cacheMap.getKey(ser.getKeyEq())));*/
        //return equal.getResult();
        //  myList.add(ObjEq(a,b,n));

        //  return List<String>
  /*          } else {
                LOGGER.log(Level.WARNING, "Не прошло валидацию =(");
                throw new Exception();
            }*/
        //  } else {

        // Equation findObjEqu = new Equation();
        //     Equation keyString = cacheMap.getKey(id);
        //  findObjEqu.setResual(keyString);
        //     myList.add(keyString);

        //  return myList;
        //    }


    }




    public static boolean isNumeric(String str) {
        try {
            Integer d = Integer.parseInt(str);
        } catch (NumberFormatException nfe) {
            return false;
        }
        return true;
    }

    public static List<Equation> ObjEqListFirst(List<String> a, List<String> b, List<String> n) {
        List<Equation> listEq = new ArrayList<Equation>();
        for (int i = 0; i < a.size(); i++) {

            String numA = a.get(i);
            String numB = b.get(i);
            String numN = n.get(i);

            Integer a1 = Integer.parseInt(numA);
            Integer b1 = Integer.parseInt(numB);
            Integer n1 = Integer.parseInt(numN);

            Equation equal = new Equation(a1, b1, n1);
            // return equal;
            listEq.add(equal);

        }
        return listEq;
        // return null;
    }

    public static Equation ObjEqList(Equation ser) {

        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        // List <Equation> listEq = new ArrayList<Equation>();

        String numA = ser.getA().toString();
        String numB = ser.getB().toString();
        String numN = ser.getN().toString();

        if (isNumeric(numA) && isNumeric(numB) && isNumeric(numN)) {
            LOGGER.log(Level.INFO, "Прошло валидацию =)");
            Integer a1 = Integer.parseInt(numA);
            Integer b1 = Integer.parseInt(numB);
            Integer n1 = Integer.parseInt(numN);

            Equation equal = new Equation(a1, b1, n1);
            return equal;
            //  listEq.add(equal);
        } else {
            LOGGER.log(Level.WARNING, "Не прошло валидацию =(");
            throw new Exception();
        }

        // return listEq;
        // return null;
    }

    private static String doListId(String a, String b, String n) {
        String id = a + b + n;
        return id;
    }

    private static List<String> idList(List<Equation> megaList) {

        List<String> idList = new ArrayList<String>();
        idList.addAll(megaList.stream().map(ser -> doListId(ser.getA().toString(), ser.getB().toString(), ser.getN().toString())).collect(Collectors.toList()));
       /*
        for (int i = 0; i < a.size(); i++) {
            String idA = a.get(i);
            String idB = b.get(i);
            String idN = n.get(i);
            idList.add(doListId(idA,idB,idN));
        }*/
        return idList;
    }


}



