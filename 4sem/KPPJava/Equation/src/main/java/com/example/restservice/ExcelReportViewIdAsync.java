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

public class ExcelReportViewIdAsync {
    public static List<String> list;

    ExcelReportViewIdAsync(List<String> l) throws ParseException {
        this.list = l;
        doExel(list);
    }

    ;

    public static void doExel(List<String> li) throws ParseException {
        Logger log;
        log = Logger.getLogger(Controller.class.getName());

        HSSFWorkbook workbook = new HSSFWorkbook();

        HSSFSheet sheet = workbook.createSheet("Common List");

        List<String> dataList = li;

        // счетчик для строк
        int rowNum = 0;

        // создаем подписи к столбцам (это будет первая строчка в листе Excel файла)
        Row row = sheet.createRow(rowNum);
        row.createCell(0).setCellValue("ID");

        // заполняем лист данными
        for (String dataModel : dataList) {
            createSheetHeader(sheet, ++rowNum, dataModel);
        }

        // запись в док
        try (FileOutputStream out = new FileOutputStream(ExlsFile.myFile)) {
            workbook.write(out);
            log.log(Level.INFO, "Excel файл создан, все ок =) !");
        } catch (IOException e) {
            log.log(Level.WARNING, "Закройте файл и попробуйте снова");
            e.printStackTrace();
        }

    }

    private static void createSheetHeader(HSSFSheet sheet, int rowNum, String dataModel) {
        Row row = sheet.createRow(rowNum);
        row.createCell(0).setCellValue(dataModel);
    }
}

