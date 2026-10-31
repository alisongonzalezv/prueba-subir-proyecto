#ifndef CARRITO_H
#define CARRITO_H

#include "Producto.h"
#include <vector>
#include <memory>
#include <algorithm>

struct ItemCarrito {
    Producto producto;
    int cantidad;

    double obtenerSubtotal() const {
        return producto.getPrecio() * cantidad;
    }
};

class Carrito {
private:
    std::vector<ItemCarrito> items;
    double porcentajeDescuento; // Ej: 5.0 para 5%
    bool aplicarIVA;            // true para 13%

public:
    Carrito() : porcentajeDescuento(0.0), aplicarIVA(true) {}

    void agregarProducto(const Producto& producto, int cantidad) {
        for (auto& item : items) {
            if (item.producto.getId() == producto.getId()) {
                item.cantidad += cantidad;
                return;
            }
        }
        items.push_back({producto, cantidad});
    }

    void quitarProducto(const QString& idProducto) {
        items.erase(
            std::remove_if(items.begin(), items.end(),
                [&idProducto](const ItemCarrito& item) {
                    return item.producto.getId() == idProducto;
                }),
            items.end()
        );
    }

    void limpiar() {
        items.clear();
        porcentajeDescuento = 0.0;
        aplicarIVA = true;
    }

    const std::vector<ItemCarrito>& getItems() const { return items; }

    void setDescuento(double descuento) { porcentajeDescuento = descuento; }
    void setAplicarIVA(bool estado) { aplicarIVA = estado; }

    // Cálculos Monetarios
    double calcularSubtotalBruto() const {
        double subtotal = 0.0;
        for (const auto& item : items) {
            subtotal += item.obtenerSubtotal();
        }
        return subtotal;
    }

    double calcularMontoDescuento() const {
        return calcularSubtotalBruto() * (porcentajeDescuento / 100.0);
    }

    double calcularMontoIVA() const {
        if (!aplicarIVA) return 0.0;
        double baseImponible = calcularSubtotalBruto() - calcularMontoDescuento();
        return baseImponible * 0.13; // 13% IVA Costa Rica
    }

    double calcularTotalNeto() const {
        return (calcularSubtotalBruto() - calcularMontoDescuento()) + calcularMontoIVA();
    }
};

#endif // Fin carrito