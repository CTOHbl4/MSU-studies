import numpy as np

def get_dominant_eigenvalue_and_eigenvector(data, num_steps):
    """
    data: np.ndarray – symmetric diagonalizable real-valued matrix
    num_steps: int – number of power method steps
    
    Returns:
    eigenvalue: float – dominant eigenvalue estimation after `num_steps` steps
    eigenvector: np.ndarray – corresponding eigenvector estimation
    """
    ### YOUR CODE HERE
    r0 = np.random.normal(10., 3., data.shape[0])
    norm = np.linalg.norm(r0)
    r1 = data.dot(r0)/norm
    mu = r0.dot(r1)/norm
    delta = 1
    j = 0
    while delta > 0.1:
        r0 = np.random.normal(j * (-1)**j, 5., data.shape[0])
        for _ in range(num_steps):
            r0 = r1
            norm = np.linalg.norm(r0)
            r1 = data.dot(r0)/norm
            mu = r0.dot(r1)/norm
            
        r0 /= norm
        r1 /= norm
        delta = np.linalg.norm(r0-r1)
        j += 3
    return float(mu), r1