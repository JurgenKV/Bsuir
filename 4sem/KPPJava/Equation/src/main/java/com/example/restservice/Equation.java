package com.example.restservice;

import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.LinkedList;

import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.Configuration;
import org.springframework.stereotype.Component;
import org.springframework.stereotype.Service;


//@Component
//@Service
public class Equation {

    private Integer a;
    private Integer b;
    private Integer n;
    public String result;
    private String key;

    private String maxRes;
    private String minRes;
    private String avgRes;

    public Equation() {
    }

    public Equation(int a, int b, int n) {
        this.a = a;
        this.b = b;
        this.n = n;

        this.result = getRes();
        this.key = getKeyEq();

    }

    public Integer getA() {
        return a;
    }

    public Integer getB() {
        return b;
    }

    public Integer getN() {
        return n;
    }
    public String getKeyEq(){
        String id = a.toString() + b.toString() + n.toString();
        return id;
    }

    public String getResult() {
        return result;
    }

    public Integer setA(Integer a) {
        return this.a = a;
    }

    public Integer setB(Integer b) {
        return this.b = b;
    }

    public Integer setN(Integer n) {
        return this.n = n;
    }

    public String setMax(String max) {
        return this.maxRes = max;
    }

    public String setMin(String min) {
        return this.minRes = min;
    }

    public String setAvg(String avg) {
        return this.avgRes = avg;
    }

    public String getMax() {
        return this.maxRes;
    }

    public String getMin() {
        return this.minRes;
    }

    public String getAvg() {
        return this.avgRes;
    }


    public String setResual(String res) {
        return this.result = res;
    }

    public String getterRes()
    {
        return result;
    }

    public double f(double x) {
        return Math.sin(x);
    }

    public double I(double a, double b, int n, double y) {
        return ((b - a) / (2 * n) * y);
    }

    public String getRes() {
        Logger LOGGER;
        LOGGER = Logger.getLogger(Controller.class.getName());
        int n;
        double a, b, dy, In, y = 0;
        a = getA();
        b = getB();
        n = getN();
        if (n > 1) {
            dy = (b - a) / n;
            y += f(a) + f(b);
            for (int i = 1; i < n; i++) {
                y += 2 * (f(a + dy * i));
            }
            In = I(a, b, n, y);
            String res;
            LOGGER.log(Level.INFO, "Вычисления выполнены успешно");
            return res = Double.toString(In);
        } else {
            LOGGER.log(Level.WARNING, "Ошибка вычисления");
            return "Error";
        }
    }
}

/*@Configuration
@ComponentScan(basePackageClasses = Equation.class)
class CalculationResults {

    *//*  public CalculationResults(int a, int b, int n, String result) {
          super(a, b, n, result);
      }*//*
    private Integer a;
    private Integer b;
    private Integer n;
    private String result;

    public CalculationResults(int a, int b, int n, String result) {
        Map<Integer, Equation> data = new HashMap<Integer, Equation>();


      this.a =a;
      this.b =b;
      this.n =n;
      this.result = result;
              data.put("1", Equation());

    }
}*/


///////////////////////
