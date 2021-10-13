import keras

def get_w_b(model):
    ws = []
    bs = []
    for i, layer in enumerate(model.layers):
        if type(layer) == keras.layers.core.Dense:
            w = layer.get_weights()[0].T
            b = layer.get_weights()[1]
            ws.append(w)
            bs.append(b)
    return ws, bs


