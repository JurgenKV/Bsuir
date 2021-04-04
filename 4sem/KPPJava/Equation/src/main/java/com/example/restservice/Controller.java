package com.example.restservice;

import java.lang.Double;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.thymeleaf.util.StringUtils;
import java.util.logging.Logger;
import java.util.logging.Level;

@RestController
public class Controller {
    @GetMapping("/equation")
    public Equation eq(@RequestParam(value = "a", defaultValue = "1") String a,
                       @RequestParam(value = "b", defaultValue = "2") String b,
                       @RequestParam(value = "n", defaultValue = "1") String n,
                       String result) {
        // 500
    /*  Integer a1 = Integer.parseInt(a);
        Integer b1 = Integer.parseInt(b);
        Integer n1 = Integer.parseInt(n);
    */  ////////////////////////////////////////
        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        if(isNumeric(a) && isNumeric(b) && isNumeric(n))
        {
            LOGGER.log(Level.INFO,"Прошло валидацию =)");
            Integer a1 = Integer.parseInt(a);
            Integer b1 = Integer.parseInt(b);
            Integer n1 = Integer.parseInt(n);

            return new Equation(a1, b1, n1, result);
        }
        else {
            LOGGER.log(Level.WARNING, "Не прошло валидацию =(");
            throw new Exception();
        }
    }

    public static boolean isNumeric(String str)
    {
        try
        {
            Integer d = Integer.parseInt(str);
        }
        catch(NumberFormatException nfe)
        {
            return false;
        }
        return true;
    }
}


