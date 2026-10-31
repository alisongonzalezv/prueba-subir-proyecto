#ifndef VENTA_H
#define VENTA_H

#include <QString>
#include <QDateTime>
#include <vector>

struct LineaVenta {
    QString idProducto;
    QString nombreProducto; // Copia estática para conservar historial si el producto cambia
    double precioHistorico;
    int cantidad;
    double subtotal;
};

class Venta {
private:
    QString folio;
    QDateTime fecha;
    QString idCajero;
    QString nombreCajero;
    double subtotalBruto;
    double montoDescuento;
    double montoIVA;
    double totalNeto;
    double efectivoRecibido;
    double cambio;
    QString estado; // "COMPLETADA" o "CANCELADA"
    std::vector<LineaVenta> lineas;

public:
    Venta() = default;

    Venta(const QString& folio, const QDateTime& fecha, const QString& idCajero,
          const QString& nombreCajero, double subtotalBruto, double montoDescuento,
          double montoIVA, double totalNeto, double efectivoRecibido, double cambio,
          const std::vector<LineaVenta>& lineas, const QString& estado = "COMPLETADA")
        : folio(folio), fecha(fecha), idCajero(idCajero), nombreCajero(nombreCajero),
          subtotalBruto(subtotalBruto), montoDescuento(montoDescuento), montoIVA(montoIVA),
          totalNeto(totalNeto), efectivoRecibido(efectivoRecibido), cambio(cambio),
          estado(estado), lineas(lineas) {}

    // Getters
    QString getFolio() const { return folio; }
    QDateTime getFecha() const { return fecha; }
    QString getIdCajero() const { return idCajero; }
    QString getNombreCajero() const { return nombreCajero; }
    double getSubtotalBruto() const { return subtotalBruto; }
    double getMontoDescuento() const { return montoDescuento; }
    double getMontoIVA() const { return montoIVA; }
    double getTotalNeto() const { return totalNeto; }
    double getEfectivoRecibido() const { return efectivoRecibido; }
    double getCambio() const { return cambio; }
    QString getEstado() const { return estado; }
    const std::vector<LineaVenta>& getLineas() const { return lineas; }

    void setEstado(const QString& nuevoEstado) { estado = nuevoEstado; }
};

#endif // VENTA_H