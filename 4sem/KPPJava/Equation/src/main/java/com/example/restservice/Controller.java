package com.example.restservice;

import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.apache.log4j.Logger;

import java.text.ParseException;
import java.util.*;
import java.util.concurrent.CompletableFuture;
import java.util.stream.Collectors;

import static org.apache.log4j.Level.INFO;

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
                             ArrayList<Equation> result) throws InterruptedException, ParseException {
        // System.out.println(result);

        CompletableFuture.runAsync(() -> {
            List<String> myListId = ObjEqListId(a, b, n);
            try {
                ExcelReportViewIdAsync d = new ExcelReportViewIdAsync(myListId);
            } catch (ParseException e) {
                e.printStackTrace();
            }
            //       ExcelReportView f = new ExcelReportView();

        });

        Thread.sleep(10000);
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

        LOGGER.log(INFO, "min/max/avg/maxA вычислены");

     /*   public ModelAndView getExcel() {
            new ModelAndView(new ExcelReportView(), "equationList", resList);
        }*/
        //Делаем Excel file
        ExcelReportView f = new ExcelReportView(resList);
        //  getExcel(resList);
        return resList;
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


        if (a.size() == b.size() && b.size() == n.size()) {

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
        } else {
            throw new Exception();
        }
    }

    public static List<String> ObjEqListId(List<String> a, List<String> b, List<String> n) {

        List<String> listEqId = new ArrayList<String>();

        if (a.size() == b.size() && b.size() == n.size()) {

            for (int i = 0; i < a.size(); i++) {

                String numA = a.get(i);
                String numB = b.get(i);
                String numN = n.get(i);
                if (isNumeric(numA) && isNumeric(numB) && isNumeric(numN)) {
                    String id = numA + numB + numN;
                    listEqId.add(id);
                } else {
                    Logger.getLogger("Не прошло валидацию =(");
                    throw new Exception();
                }
            }
        } else {
            throw new Exception();
        }

        return listEqId;
    }

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
