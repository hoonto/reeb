import numpy as np

def make_isotropic_gaussian_kernel(sigma):
    """
    Makes an isotropic gaussian kernel function with bandwidth sigma.
    """
    # the kernel function is a function of ||x-y||^2, the squared Euclidean
    # distance between x and y
    def kernel(d_squared):
        return 1/np.sqrt(2*np.pi*sigma**2)*np.exp(-1/2.*d_squared/sigma**2)

    return kernel


def kernel_density_estimate(x, data, kernel):
    """
    Estimates the probability density at x, given the data and the kernel.
    This obviously does not use data-dependent kernels.
    """
    # first, we calculate the interpoint distances
    d_squared = np.sum((data - x)**2, axis=1)

    # and evaluate the kernel at each distance
    weights = kernel(d_squared)

    # return the kernel density estimate
    return np.mean(weights)


def gradient(x, data, kernel, sigma):
    """
    Computes the gradient at x of the kde of the data using isotropic Guassian
    kernels with bandwidth sigma.
    """
    # the number of data points and the dimensionality
    n,d = data.shape 

    # first, we form the u_i
    u = 1./(sigma**2)*(data - x)

    # now we form the c_i
    d_squared = np.sum((data - x)**2, axis=1)
    c = kernel(d_squared)

    return -1. * np.mean(c * u.T, axis=1)


def hessian(x, data, kernel, sigma):
    """
    Computes the Hessian at x of the kde of the data.
    """
    # the number of data points and the dimensionality
    n,d = data.shape 

    # first, we form the u_i
    u = 1./(sigma**2)*(data - x)

    # now we form the c_i
    d_squared = np.sum((data - x)**2, axis=1)
    c = kernel(d_squared)

    Z = 1/sigma**2 * np.eye(d)

    return 1./n * (c*u.T).dot(u) - 1./n * Z * np.sum(c)


def local_inv_cov(x, data, kernel, sigma):
    """
    Computes the local inverse covariance from the gradient and hessian.
    """
    p = kernel_density_estimate(x, data, kernel)
    h = hessian(x, data, kernel, sigma)
    g = gradient(x, data, kernel, sigma)[:,np.newaxis]

    return -1./p * h + 1./p**2 * g.dot(g.T)


def mean_shift_update(x, data, kernel):
    """
    Applies a kernel mean shift update to the point x, given the data and a
    1-d kernel function of the squared distance.
    """
    # first, calculate the interpoint distance
    d_squared = np.sum((data - x)**2, axis=1)

    # eqvaluate the kernel at each distance
    weights = kernel(d_squared)

    # now reweight each point
    shift = (data.T.dot(weights))/np.sum(weights)

    # return the shift update
    return shift - x


def subspace_constrained_mean_shift_update(x, data, kernel, sigma):
    """
    Compute the constrained mean shift update of the point at x.
    """
    # first, we evaluate the mean shift update
    msu = mean_shift_update(x, data, kernel)

    # next we compute the local inverse covariance
    lic = local_inv_cov(x, data, kernel, sigma)

    # now we perform the eigendecomposition of lic
    vals, vects = np.linalg.eig(lic)

    # sort the eigenvectors by increasing eigenvalue
    idx = np.argsort(vals)
    vects = vects[:, idx]

    # project the mean shift update onto the orthogonal subspace spanned
    # by the largest eigenvectors of the inverse covariance
    V = vects[:,1:]
    return V.dot(V.T).dot(msu)


def scms(data, sigma, n_iterations=5):
    """
    Performs subspace constrained mean shift on the entire data set using
    a Gaussian kernel with bandwidth of sigma.
    """
    denoised = data.copy()
    kernel = make_isotropic_gaussian_kernel(sigma)
    for j in range(n_iterations):
        for i in range(data.shape[0]):
            denoised[i] += subspace_constrained_mean_shift_update(
                    denoised[i], data, kernel, sigma)

    return denoised
