import scms
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

# development
reload(scms)

# PARAMETERS
# =============================================================================

# data sampling
n_manifold = 300
n_bg = 200
sampling_sigma = .2

# mean shift
kernel_sigma = .5
mean_shift_iterations = 5

show_plots = True

# =============================================================================

# sample the manifold
manifold_x = np.linspace(0, 4*np.pi, n_manifold)
manifold_y = np.sin(manifold_x)

manifold_x += np.random.normal(0, sampling_sigma, n_manifold)
manifold_y += np.random.normal(0, sampling_sigma, n_manifold)

manifold = np.column_stack((manifold_x, manifold_y))

# add some background noise
bg_x = np.random.uniform(-1, 14, n_bg)
bg_y = np.random.uniform(-2, 2, n_bg)
bg = np.column_stack((bg_x, bg_y))

# combine the noise and the signal
data = np.vstack((manifold, bg))

# plot
if show_plots:
    plt.scatter(*zip(*data))
    plt.title("The raw data sampled from the manifold")
    plt.show()

# test out mean shift
kernel = scms.make_isotropic_gaussian_kernel(kernel_sigma)

shifted = data.copy()
for j in range(mean_shift_iterations):
    for i in range(data.shape[0]): 
        shifted[i] += scms.mean_shift_update(shifted[i], data, kernel)

if show_plots:
    plt.scatter(*zip(*shifted))
    plt.title("Simple mean shift for {} iterations".format(
            mean_shift_iterations))
    plt.show()

denoised = scms.scms(data, kernel_sigma)

plt.scatter(*zip(*denoised))
plt.show()


