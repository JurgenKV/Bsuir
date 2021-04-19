package com.example.restservice;


import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.poi.hssf.usermodel.HSSFFont;
import org.apache.poi.hssf.usermodel.HSSFRow;
import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.hssf.util.HSSFColor;
import org.apache.poi.ss.usermodel.*;
import org.apache.log4j.Logger;
import org.springframework.web.servlet.view.document.AbstractXlsView;

public class ExcelReportView extends AbstractXlsView{

    @Override
    protected void buildExcelDocument(Map<String, Object> model, Workbook workbook, HttpServletRequest request,
                                      HttpServletResponse response) throws Exception {

        response.setHeader("Content-Disposition", "attachment;filename=\"EquationTable.xls\"");
        List<Equation> equationList = (List<Equation>) model.get("equationList");
        Sheet sheet = workbook.createSheet("Integral sq");
        Row header = sheet.createRow(0);
        header.createCell(0).setCellValue("A");
        header.createCell(1).setCellValue("B");
        header.createCell(2).setCellValue("N");
        header.createCell(3).setCellValue("res");

        int rowNum = 1;
        for(Equation Eq:equationList){
            Row row = sheet.createRow(rowNum++);
            row.createCell(0).setCellValue(Eq.getA());
            row.createCell(1).setCellValue(Eq.getB());
            row.createCell(2).setCellValue(Eq.getN());
            row.createCell(3).setCellValue(Eq.getResult());
            System.out.println("========================================");
        }

    }
}

