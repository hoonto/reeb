import numpy as np

def noisy_sine(n_manifold=200, n_bg=100, manifold_sigma=.2):
    # sample the manifold
    manifold_x = np.linspace(0, 4*np.pi, n_manifold)
    manifold_y = np.sin(manifold_x)

    manifold_x += np.random.normal(0, manifold_sigma, n_manifold)
    manifold_y += np.random.normal(0, manifold_sigma, n_manifold)

    manifold = np.column_stack((manifold_x, manifold_y))

    # add some background noise
    bg_x = np.random.uniform(-1, 14, n_bg)
    bg_y = np.random.uniform(-2, 2, n_bg)
    bg = np.column_stack((bg_x, bg_y))

    # combine the noise and the signal
    data = np.vstack((manifold, bg))

    return data


def noisy_diamond(n_manifold=50, n_bg=100, sigma=0.2):
    data = np.empty((0,2))
    add_data = lambda x,y : np.vstack((data, np.column_stack((x,y))))

    # left
    x = np.random.uniform(-3,-1,n_manifold*1.5)
    y = np.zeros_like(x) + np.random.normal(0, sigma, n_manifold*1.5)
    data = add_data(x,y)

    # NW
    x = np.random.uniform(-1,0,n_manifold)
    y = x + 1 + np.random.normal(0, sigma, n_manifold)
    data = add_data(x,y)

    # SW
    x = np.random.uniform(-1,0,n_manifold)
    y = -x - 1 + np.random.normal(0, sigma, n_manifold)
    data = add_data(x,y)

    # NE
    x = np.random.uniform(0,1,n_manifold)
    y = -x + 1 + np.random.normal(0, sigma, n_manifold)
    data = add_data(x,y)

    # SE
    x = np.random.uniform(0,1,n_manifold)
    y = x - 1 + np.random.normal(0, sigma, n_manifold)
    data = add_data(x,y)

    # right
    x = np.random.uniform(1,3,n_manifold*1.5)
    y = np.zeros_like(x) + np.random.normal(0, sigma, n_manifold*1.5)
    data = add_data(x,y)

    # background noise
    x = np.random.uniform(-3,3,n_bg)
    y = np.random.uniform(-2,2,n_bg)
    data = add_data(x,y)

    return data
