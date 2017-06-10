package main;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigDecimal;

import euler.Velocity;

/**
 * @author Elisa Boselli
 *
 */
public class Parachute {

  static BigDecimal GRAVITY = new BigDecimal("0.1");
  static BigDecimal RESISTANCE_COEFFICIENT = new BigDecimal("0.5");
  static BigDecimal MASS = new BigDecimal("1.0");

  static BigDecimal TIME = new BigDecimal("1.0");
  static BigDecimal EULER_STEP = new BigDecimal("0.02");
  final static String FILE = "parachute.dat";

  /**
   * Main method.
   * 
   * @param args
   * @throws IOException
   */
  public static void main(String[] args) throws IOException {
    if (args.length == 5) {
      GRAVITY = new BigDecimal(args[0]);
      RESISTANCE_COEFFICIENT = new BigDecimal(args[1]);
      MASS = new BigDecimal(args[2]);
      EULER_STEP = new BigDecimal(args[3]);
      TIME = new BigDecimal(args[4]);
    }

    Boolean firstTime = true;

    BigDecimal t = new BigDecimal("0");
    t = t.setScale(2, BigDecimal.ROUND_HALF_UP);
    BigDecimal velocity = new BigDecimal("0");
    velocity = velocity.setScale(5, BigDecimal.ROUND_HALF_EVEN);

    File file = new File(FILE);
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = new FileWriter(file);

    BufferedWriter bw = new BufferedWriter(fw);

    Velocity vFunc = new Velocity(GRAVITY, RESISTANCE_COEFFICIENT, MASS);

    while (t.compareTo(TIME) < 1) {
      if (firstTime) {
        System.out.println("(Time,Velocity)");
        bw.write("#Time   Velocity");
        bw.newLine();
        firstTime = false;
      } else {
        velocity = vFunc.applyEuler(EULER_STEP, velocity);
        velocity = velocity.setScale(5, BigDecimal.ROUND_HALF_EVEN);
      }
      System.out.println("(" + t + "," + velocity + ")");
      bw.write(t + " " + velocity);
      bw.newLine();
      t = t.add(EULER_STEP);
    }
    bw.close();
  }
}
