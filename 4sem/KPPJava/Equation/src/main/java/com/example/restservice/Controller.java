package com.example.restservice;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.beans.factory.annotation.Autowired;

import java.util.logging.Logger;
import java.util.logging.Level;

@RestController
public class Controller {
    @Autowired
    private final CacheMap cacheMap;
  //  @Autowired
    //Semaphor sema;

    public Controller(CacheMap cacheMap) {
        this.cacheMap = cacheMap;
    }

    @GetMapping("/equation")
    public String eq(@RequestParam(value = "a", defaultValue = "1") String a,
                     @RequestParam(value = "b", defaultValue = "2") String b,
                     @RequestParam(value = "n", defaultValue = "1") String n,
                     String result) throws InterruptedException {

        Semaphor.sem.release();
        //////////////////////////////////////// 500
    /*  Integer a1 = Integer.parseInt(a);
        Integer b1 = Integer.parseInt(b);
        Integer n1 = Integer.parseInt(n);
    */  ////////////////////////////////////////
        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        String id = a + b + n;
        if (!cacheMap.isStored(id)) {

            if (isNumeric(a) && isNumeric(b) && isNumeric(n)) {
                LOGGER.log(Level.INFO, "Прошло валидацию =)");
                Integer a1 = Integer.parseInt(a);
                Integer b1 = Integer.parseInt(b);
                Integer n1 = Integer.parseInt(n);
                Equation equal = new Equation(a1, b1, n1);
                //  Thread.sleep(1000 * 3);
                cacheMap.insert(id, equal.getRes());

                return equal.getResult();
            } else {
                LOGGER.log(Level.WARNING, "Не прошло валидацию =(");
                throw new Exception();
            }
        } else {
            Equation findObjEqu = new Equation();
            String keyString = cacheMap.getKey(id);
            findObjEqu.setResual(keyString);
            return keyString;
        }

    }

    public static boolean isNumeric(String str) {
        try {
            Integer d = Integer.parseInt(str);
        } catch (NumberFormatException nfe) {
            return false;
        }
        return true;
    }
}


