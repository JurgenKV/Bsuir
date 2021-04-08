package com.example.restservice;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import java.util.logging.Logger;
import java.util.logging.Level;

class GreetingTest {

    @Test
    void TestingTrue()
    {
        Logger LOGGER;
        LOGGER = Logger.getLogger(GreetingTest.class.getName());
             Equation test = new Equation(2, 4, 4);
             double result1 = 0.2325281951165403;
             String result2 = test.getRes();
             Double result = Double.parseDouble(result2);
             Assertions.assertEquals(result1, result);
        LOGGER.log(Level.INFO,"Test (True)");
    }

   @Test
    void TestingFalse()
    {
        Logger LOGGER;
        LOGGER = Logger.getLogger(GreetingTest.class.getName());
            Equation test = new Equation(2, 4, 4);
            double result1 = 1.0;
            String result2 = test.getRes();
            Double result = Double.parseDouble(result2);
            Assertions.assertNotEquals(result1, result);
        LOGGER.log(Level.INFO,"Test (False)");
    }
}