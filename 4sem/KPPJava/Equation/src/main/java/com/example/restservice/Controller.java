package com.example.restservice;

import org.apache.poi.ss.formula.functions.T;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.servlet.ModelAndView;
import org.thymeleaf.util.ListUtils;
import org.apache.log4j.Logger;
import java.lang.reflect.Array;
import java.util.*;
import java.util.logging.Level;
import java.util.stream.Collectors;
import java.util.stream.LongStream;
import java.util.stream.Stream;

@Component
@RestController
public class Controller {
    @Autowired
    private final CacheMap cacheMap;

    @Autowired
    MathLab math;

    public Controller(CacheMap cacheMap) {
        this.cacheMap = cacheMap;
    }

    @GetMapping("/equation")
    public List<Equation> eq(@RequestParam(value = "a", defaultValue = "1") List<String> a,
                             @RequestParam(value = "b", defaultValue = "2") List<String> b,
                             @RequestParam(value = "n", defaultValue = "1") List<String> n,
                             ArrayList<Equation> result) throws InterruptedException {
        // System.out.println(result);
        List<Equation> myList = new ArrayList<Equation>();

        Semaphor.sem.release();
        //////////////////////////////////////// 500
    /*  Integer a1 = Integer.parseInt(a);
        Integer b1 = Integer.parseInt(b);
        Integer n1 = Integer.parseInt(n);
    */  ////////////////////////////////////////
        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        myList = ObjEqListFirst(a, b, n);
        List<Equation> resList = new ArrayList<Equation>();


        resList.addAll(myList.stream()
                .parallel()
                .filter(cacheMap::isStored)
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


        // min / max / avg / maxA


        Comparator<Equation> comparatorMax = Comparator.comparing(Equation::getResult);
        Comparator<Equation> comparatorMin = Comparator.comparing(Equation::getResult);
        Comparator<Equation> comparatorAvg = Comparator.comparing(Equation::getKeyEq);
        Comparator<Equation> comparatorMaxA = Comparator.comparing(Equation::getA);



        Equation minObject = resList.stream().min(comparatorMin).get();
        Equation maxObject = resList.stream().max(comparatorMax).get();
        double avgObject = resList.stream().mapToDouble(ser -> Double.parseDouble(ser.getResult())).average().getAsDouble();
        Equation maxAObj = resList.stream().max(comparatorMaxA).get();

        math.setMax(maxObject.result);
        math.setMin(minObject.result);
        math.setAvg(Double.toString(avgObject));
        math.setMaxA(maxAObj.result);

        System.out.println("maxObject = " + maxObject.result);
        System.out.println("minObject = " + minObject.result);
        System.out.println("AVG = " + avgObject);
        System.out.println("maxAObj = " + maxAObj.result);

     /*   public ModelAndView getExcel() {
            new ModelAndView(new ExcelReportView(), "equationList", resList);
        }*/
        getExcel(resList);
        return resList;
    }

    @RequestMapping(method=RequestMethod.GET)
    public ModelAndView getExcel(List<Equation> L){
        System.out.println("adasdasdas");
        return new ModelAndView(new ExcelReportView(), "equationList", L);
    }
/*

   public class ReportController {
        @Autowired
        List<Equation> resList;
        @RequestMapping(method= RequestMethod.GET)

        public ModelAndView getExcel(){

            return new ModelAndView(new ExcelReportView(), "equationList", resList);
        }
    }
*/


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



        if(a.size() == b.size() && b.size() == n.size()) {

            for (int i = 0; i < a.size(); i++) {

                String numA = a.get(i);
                String numB = b.get(i);
                String numN = n.get(i);
                if (isNumeric(numA) && isNumeric(numB) && isNumeric(numN)) {
                    Integer a1 = Integer.parseInt(numA);
                    Integer b1 = Integer.parseInt(numB);
                    Integer n1 = Integer.parseInt(numN);

                    Equation equal = new Equation(a1, b1, n1);
                    listEq.add(equal);

                } else {
                    Logger.getLogger("Не прошло валидацию =(");
                    throw new Exception();

                }
            }

            return listEq;
        }else
        {
            throw new Exception();
        }
    }

/*
    public static Equation ObjEqList(Equation ser) {

        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());

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
    }*/

    private static String doListId(String a, String b, String n) {
        String id = a + b + n;
        return id;
    }

    private static List<String> idList(List<Equation> megaList) {

        List<String> idList = new ArrayList<String>();
        idList.addAll(megaList.stream().map(ser -> doListId(ser.getA().toString(), ser.getB().toString(), ser.getN().toString())).collect(Collectors.toList()));
        return idList;
    }


}


/*
@RequestMapping("/report")
class ReportController {
    @RequestMapping(method=RequestMethod.GET)
    public ModelAndView getExcel(){
        System.out.println("Хоба");
        List<Equation> studentList = new ArrayList<Equation>();
        studentList.add(new Equation(2, 3, 4));
        studentList.add(new Equation(5, 6, 7));
        return new ModelAndView(new ExcelReportView(), "equationList", studentList);
    }
}
*/



