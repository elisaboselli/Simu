package euler;

import java.math.BigDecimal;

/**
 * @author Elisa Boselli
 *
 */
public final class Velocity extends Euler {

  private BigDecimal gravity;
  private BigDecimal resistanceCoefficient;
  private BigDecimal mass;

  /**
   * Constructor of the class - returns a Velocity object.
   * 
   * @param g
   *          - Gravity.
   * @param rc
   *          - Resistance Coefficient.
   * @param m
   *          - Mass.
   * @throws IllegalArgumentException
   *           if any parameter is negative.
   */
  public Velocity(BigDecimal g, BigDecimal rc, BigDecimal m) throws IllegalArgumentException {
    this.gravity = g;
    this.resistanceCoefficient = rc;
    this.mass = m;
    if (!repOk())
      throw new IllegalArgumentException();
  }

  /**
   * Apply the Velocity's derivative.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @return (BigDecimal) Derivative application.
   */
  @Override
  protected BigDecimal applyDerivative(BigDecimal previousVelocity) throws IllegalArgumentException {
    BigDecimal result = gravity.subtract(((resistanceCoefficient.divide(mass))
        .multiply(previousVelocity)));
    return result;
  }

  /**
   * Check if all the parameters are not negative.
   * 
   * @return (Boolean) True if any parameter is negative, otherwise false.
   */
  private boolean repOk() {
    BigDecimal zero = new BigDecimal("0");
    return (gravity.compareTo(zero) > 0) && (resistanceCoefficient.compareTo(zero) > 0)
        && (mass.compareTo(zero) > 0);
  }

}