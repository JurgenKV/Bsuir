package com.example.restservice;


import org.apache.poi.hssf.usermodel.HSSFSheet;
import org.apache.poi.hssf.usermodel.HSSFWorkbook;
import org.apache.poi.ss.usermodel.Row;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.ParseException;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ExcelReportView {
    public static List<Equation> list;

    ExcelReportView(List<Equation> l) throws ParseException {
        this.list = l;
        doExel(list);
    }

    ;

    public static void doExel(List<Equation> li) throws ParseException {
        Logger log;
        log = Logger.getLogger(Controller.class.getName());

        HSSFWorkbook workbook = new HSSFWorkbook();

        HSSFSheet sheet = workbook.createSheet("Common List");

        List<Equation> dataList = li;

        // счетчик для строк
        int rowNum = 0;

        // создаем подписи к столбцам (это будет первая строчка в листе Excel файла)
        Row row = sheet.createRow(rowNum);
        row.createCell(0).setCellValue("ID");
        row.createCell(1).setCellValue("A");
        row.createCell(2).setCellValue("B");
        row.createCell(3).setCellValue("N");
        row.createCell(4).setCellValue("Result");

        // заполняем лист данными
        for (Equation dataModel : dataList) {
            createSheetHeader(sheet, ++rowNum, dataModel);
        }

        // запись в док
        try (FileOutputStream out = new FileOutputStream(new File("d:\\Study\\Programming\\Java\\Equation\\File.xls"))) {
            workbook.write(out);
            log.log(Level.INFO, "Excel файл создан, все ок =) !");
        } catch (IOException e) {
            log.log(Level.WARNING, "Закройте файл и попробуйте снова");
            e.printStackTrace();
        }

    }

    private static void createSheetHeader(HSSFSheet sheet, int rowNum, Equation dataModel) {
        Row row = sheet.createRow(rowNum);

        row.createCell(0).setCellValue(dataModel.getKeyEq());
        row.createCell(1).setCellValue(dataModel.getA());
        row.createCell(2).setCellValue(dataModel.getB());
        row.createCell(3).setCellValue(dataModel.getN());
        row.createCell(4).setCellValue(dataModel.getResult());
    }
}

