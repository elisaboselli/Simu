package main;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigDecimal;

/**
 * @author Elisa Boselli
 *
 */
public class BasicTesting {

  final static BigDecimal GRAVITY = new BigDecimal("0.1");
  final static BigDecimal RESISTANCE_COEFFICIENT = new BigDecimal("0.5");
  final static BigDecimal MASS = new BigDecimal("1.0");
  final static BigDecimal TIME = new BigDecimal("1.0");
  final static BigDecimal EULER_STEP = new BigDecimal("0.02");
  final static String FILE = "plot.dat";

  /**
   * Main method.
   * 
   * @param args
   * @throws IOException
   */
  public static void main(String[] args) throws IOException {
    Boolean firstTime = true;

    BigDecimal t = new BigDecimal("0");
    t = t.setScale(2, BigDecimal.ROUND_HALF_UP);
    BigDecimal previousVelocity = new BigDecimal("0");
    previousVelocity = previousVelocity.setScale(8, BigDecimal.ROUND_HALF_EVEN);
    BigDecimal currentVelocity = new BigDecimal("0");
    currentVelocity = currentVelocity.setScale(8, BigDecimal.ROUND_HALF_EVEN);

    File file = new File(FILE);
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = new FileWriter(file);

    BufferedWriter bw = new BufferedWriter(fw);

    while (t.compareTo(TIME) < 1) {
      if (firstTime) {
        System.out.println("(Time,Velocity)");
        bw.write("#Time   Velocity");
        bw.newLine();
        firstTime = false;
      } else {
        currentVelocity = applyEuler(previousVelocity);
        currentVelocity = currentVelocity.setScale(8, BigDecimal.ROUND_HALF_EVEN);
      }
      System.out.println("(" + t + "," + currentVelocity + ")");
      bw.write(t + " " + currentVelocity);
      previousVelocity = currentVelocity;
      bw.newLine();
      previousVelocity = previousVelocity.setScale(8, BigDecimal.ROUND_HALF_EVEN);
      t = t.add(EULER_STEP);
    }
    bw.close();
  }

  /**
   * Gets the next function's value using the Euler's technique.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @return (BigDecimal) Next function's value.
   */
  private static BigDecimal applyEuler(BigDecimal v) {
    return (v.add((EULER_STEP.multiply((GRAVITY.subtract(((RESISTANCE_COEFFICIENT.divide(MASS))
        .multiply(v))))))));
  }

}
