package main;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigDecimal;

import euler.Predator;
import euler.Prey;

/**
 * @author Elisa Boselli
 *
 */
public class LotkaVolterra {

  static BigDecimal PREY_GROWTH_RATE = new BigDecimal("3");
  static BigDecimal PREY_MORTALITY_RATE = new BigDecimal("2");
  static BigDecimal PREDATOR_GROWTH_RATE = new BigDecimal("1");
  static BigDecimal PREDATOR_MORTALITY_RATE = new BigDecimal("2");

  static BigDecimal TIME = new BigDecimal("10.0");
  static BigDecimal EULER_STEP = new BigDecimal("0.1");
  final static String FILE = "Lotka_Volterra.dat";

  public static void main(String[] args) throws IOException {
    if (args.length == 6) {
      PREY_GROWTH_RATE = new BigDecimal(args[0]);
      PREY_MORTALITY_RATE = new BigDecimal(args[1]);
      PREDATOR_GROWTH_RATE = new BigDecimal(args[2]);
      PREDATOR_MORTALITY_RATE = new BigDecimal(args[3]);
      EULER_STEP = new BigDecimal(args[4]);
      TIME = new BigDecimal(args[5]);
    }
    Boolean firstTime = true;

    BigDecimal t = new BigDecimal("0");
    t = t.setScale(1, BigDecimal.ROUND_HALF_UP);

    BigDecimal preyQty = new BigDecimal("2");
    preyQty = preyQty.setScale(4, BigDecimal.ROUND_HALF_EVEN);
    BigDecimal previousPreyQty = preyQty;

    BigDecimal predatorQty = new BigDecimal("2");
    predatorQty = predatorQty.setScale(4, BigDecimal.ROUND_HALF_EVEN);
    BigDecimal previousPredatorQty = predatorQty;

    File file = new File(FILE);
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = new FileWriter(file);

    BufferedWriter bw = new BufferedWriter(fw);

    Prey preyFunc = new Prey(PREY_GROWTH_RATE, PREY_MORTALITY_RATE);
    Predator predatorFunc = new Predator(PREDATOR_GROWTH_RATE, PREDATOR_MORTALITY_RATE);

    while (t.compareTo(TIME) < 1) {
      if (firstTime) {
        System.out.println("(Time,Prey,Predator)");
        bw.write("#Time   Prey   Predator");
        bw.newLine();
        firstTime = false;

      } else {

        preyQty = preyFunc.applyEuler(EULER_STEP, previousPreyQty, previousPredatorQty);
        predatorQty = predatorFunc.applyEuler(EULER_STEP, previousPredatorQty, previousPreyQty);

        preyQty = preyQty.setScale(4, BigDecimal.ROUND_HALF_EVEN);
        predatorQty = predatorQty.setScale(4, BigDecimal.ROUND_HALF_EVEN);
      }

      System.out.println("(" + t + "," + preyQty + "," + predatorQty + ")");
      bw.write(t + " " + preyQty + " " + predatorQty);
      bw.newLine();

      previousPreyQty = preyQty;
      previousPredatorQty = predatorQty;

      t = t.add(EULER_STEP);
    }
    bw.close();
  }
}
