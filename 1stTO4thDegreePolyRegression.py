import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression
from sklearn.preprocessing import PolynomialFeatures
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error

# Load the dataset
df = pd.read_csv(r"C:\Users\omarb\PycharmProjects\vFarm\preprocessed_nutrient_addition_data.csv")

# Function to fit the model and predict nutrients based on the degree of polynomial
def fit_and_predict(X, y, poly_degree, cumulative_ph_drop, current_volume):
    # Polynomial Features
    poly = PolynomialFeatures(degree=poly_degree)
    X_poly = poly.fit_transform(X)

    # Fitting the Linear Regression Model
    model = LinearRegression()
    model.fit(X_poly, y)

    # Predict nutrients
    ph_drop_poly = poly.transform(np.array([[cumulative_ph_drop]]))
    predicted_nutrients = model.predict(ph_drop_poly)[0]

    # Adjusting based on the current volume
    reference_volume = df['Current Volume (mL)'].mean()
    adjusted_nutrients = (current_volume / reference_volume) * predicted_nutrients
    return adjusted_nutrients, model, poly

# Function to display polynomial equation
def display_polynomial_equation(model, poly, reference_volume):
    coefficients = model.coef_
    intercept = model.intercept_
    polynomial_terms = [f"{coeff:.2e} * (cumulative_ph_drop)^{i}" if i > 1 else f"{coeff:.2e} * cumulative_ph_drop" for i, coeff in enumerate(coefficients[1:], start=1)]
    polynomial_equation = " + ".join(polynomial_terms)
    polynomial_equation = f"({intercept:.2e} + " + polynomial_equation + f") * (current_volume / reference_volume)"
    print("Polynomial Equation for Predicted Nutrients to Add:")
    print(polynomial_equation)

# Main execution function
def execute_prediction(poly_degree, cumulative_ph_drop, current_volume):
    if 1 <= poly_degree <= 4:
        X = df[['Cumulative pH Drop']].values  # Features
        y = df['Cumulative Nutrients Added (mL)'].values  # Target
        adjusted_nutrients, model, poly = fit_and_predict(X, y, poly_degree, cumulative_ph_drop, current_volume)
        print(f"Predicted Cumulative Nutrients to Add for degree {poly_degree}: {adjusted_nutrients:.2f} mL")
        display_polynomial_equation(model, poly, df['Current Volume (mL)'].mean())
    else:
        print("poly_degree must be between 1 and 4.")

# Example usage
poly_degree = 2  # Choose between 1 to 4
current_pH = 7.0
desired_pH = 6.8
current_volume_input = 500  # Example input for current volume in mL
cumulative_ph_drop = (current_pH - desired_pH)  # Example input for cumulative pH drop

execute_prediction(poly_degree, cumulative_ph_drop, current_volume_input)
