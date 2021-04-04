package com.example.restservice;

public class Equation
{
    private Integer a;
    private Integer b;
    private Integer n;
    private String result;

    public Equation(int a, int b, int n, String result)
    {
        this.a = a;
        this.b = b;
        this.n = n;
        this.result = getRes();
    }

    public double getA()
    {
        return a;
    }
    public double getB()
    {
        return b;
    }
    public int getN()
    {
        return n;
    }

    public  double f(double x)
    {
        return Math.sin(x);
    }
    public  double I(double a,double b,int n,double y)
    {
        return ((b-a)/(2*n)*y);
    }
    public String getRes ()
    {
        int n; double a,b,dy,In,y=0;
        a = getA();
        b = getB();
        n = getN();
        if (n>1){
            dy=(b-a)/n;
            y+=f(a)+f(b);
            for (int i=1; i<n; i++) {y+=2*(f(a+dy*i));}
            In=I(a,b,n,y);
            String res;
            return res = Double.toString(In);
        }
        else
            return "Error";
    }

   /* public class IncorrectFileExtensionException
            extends RuntimeException {
        public IncorrectFileExtensionException(String errorMessage, Throwable err) {
            super(errorMessage, err);
        }
    }*/
}
///////////////////////
