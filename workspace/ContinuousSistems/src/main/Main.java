package main;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class Main {

  final static double GRAVITY = 0.1;
  final static double RESISTANCE_COEFFICIENT = 0.5;
  final static double MASS = 1.0;
  final static double TIME = 1.0;
  final static double EULER_STEP = 0.02;
  final static String FILE = "plot.dat";

  public static void main(String[] args) throws IOException {

    double t = 0;
    double previousVelocity = 0;
    double currentVelocity = 0;

    File file = new File(FILE);
    if (!file.exists()) {
      file.createNewFile();
    }
    FileWriter fw = null;
    fw = new FileWriter(file);

    BufferedWriter bw = new BufferedWriter(fw);

    while (t <= TIME) {
      if (t == 0) {
        System.out.println("(Time,Velocity)");
        bw.write("#Time   Velocity");
        System.out.println("(" + t + "," + currentVelocity + ")");
        bw.write(t + " " + currentVelocity);
        previousVelocity = currentVelocity;
      } else {
        currentVelocity = applyEuler(previousVelocity);
        System.out.println("(" + t + "," + currentVelocity + ")");
        bw.write(t + " " + currentVelocity);
        previousVelocity = currentVelocity;
      }
      t = t + EULER_STEP;
    }
    bw.close();
  }

  private static double applyEuler(double v) {
    return (v + (EULER_STEP * (GRAVITY - ((RESISTANCE_COEFFICIENT / MASS) * v))));
  }
}
