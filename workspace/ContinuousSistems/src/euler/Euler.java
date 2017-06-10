package euler;

import java.math.BigDecimal;

/**
 * @author Elisa Boselli
 *
 */
public abstract class Euler {

  protected String derivative;

  /**
   * Gets the next function's value using the Euler's technique.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @param step
   *          - Euler's step.
   * @return (BigDecimal) Next function's value.
   */
  public BigDecimal applyEuler(BigDecimal previousValue, BigDecimal step) {
    return previousValue.add(step.multiply(applyDerivative(previousValue)));
  }

  /**
   * Apply the function's derivative.
   * 
   * @param previousValue
   *          - Previous function's value.
   * @return (BigDecimal) Derivative application.
   */
  protected abstract BigDecimal applyDerivative(BigDecimal previousValue);

}
