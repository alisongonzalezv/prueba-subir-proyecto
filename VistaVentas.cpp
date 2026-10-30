#include "VistaVentas.h"
#include "GestorJSON.h"

VistaVentas::VistaVentas(QWidget* parent) : QWidget(parent) {
    categoriaSeleccionada = "Todos";
    subtotalActual = 0.0;
    ivaActual = 0.0;
    descuentoActual = 0.0;
    totalActual = 0.0;
    contadorVentas = 1001;

    cargarCatalogoDemo();
    configurarInterfaz();
    renderizarProductos();
    renderizarCarrito();
}

void VistaVentas::cargarCatalogoDemo() {
    GestorJSON gestor;
    catalogo = gestor.cargarProductos();

    // Cargar datos semilla únicamente si el archivo JSON está vacío o no existe
    if (catalogo.empty()) {
        catalogo = {
            {"S001", "Papas Tostadas Tosty", 750.0, "Snack de papa 115g", "Snacks", 25},
            {"S002", "Platanitos Salados", 600.0, "Plátano tostado 100g", "Snacks", 15},
            {"S003", "Doritos Queso Nacho", 850.0, "Snack de maíz 120g", "Snacks", 8},
            {"S004", "Chocolate Jet", 350.0, "Barra de chocolate 12g", "Snacks", 30},
            {"S005", "Galletas Chicky", 450.0, "Galletas con cobertura", "Snacks", 12},
            {"B001", "Agua Cristal 600ml", 600.0, "Agua embotellada", "Bebidas", 50},
            {"B002", "Café Caliente 8oz", 700.0, "Café negro / con leche", "Bebidas", 40},
            {"B003", "Coca-Cola 600ml", 1100.0, "Gaseosa en botella", "Bebidas", 18},
            {"B005", "Fresco Leche Fresa", 800.0, "Dos Pinos 315ml", "Bebidas", 10},
            {"C001", "Empanada de Carne", 1200.0, "Arreglada y caliente", "Comida", 5},
            {"C002", "Empanada de Queso", 1100.0, "Arreglada y caliente", "Comida", 7},
            {"C003", "Muffin de Chispas", 950.0, "Repostería dulce", "Comida", 9},
            {"U001", "Lapicero BIC Negro", 300.0, "Punto mediano", "Útiles", 100},
            {"U002", "Lapicero BIC Azul", 300.0, "Punto mediano", "Útiles", 100},
            {"U004", "Cuaderno Universitario", 1850.0, "100 hojas cosido", "Útiles", 20},
            {"H001", "Alcohol en Gel 60ml", 950.0, "Portátil antibacterial", "Higiene", 14},
            {"H002", "Pañuelos Kleenex", 450.0, "Paquete de bolsillo", "Higiene", 22}
        };
        gestor.guardarProductos(catalogo);
    }
}

void VistaVentas::configurarInterfaz() {
    QHBoxLayout* layoutPrincipal = new QHBoxLayout(this);
    layoutPrincipal->setContentsMargins(15, 15, 15, 15);
    layoutPrincipal->setSpacing(15);
    this->setStyleSheet("background-color: #f8f9fa;");

    // PANEL IZQUIERDO: Catálogo y Búsqueda
    QWidget* panelIzquierdo = new QWidget(this);
    QVBoxLayout* layoutIzquierdo = new QVBoxLayout(panelIzquierdo);
    layoutIzquierdo->setContentsMargins(0, 0, 0, 0);
    layoutIzquierdo->setSpacing(15);

    txtBuscar = new QLineEdit(this);
    txtBuscar->setPlaceholderText("Buscar producto por nombre o código...");
    txtBuscar->setStyleSheet(
        "QLineEdit { background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 10px; "
        "padding: 10px 15px; font-size: 13px; color: #111827; }"
        "QLineEdit:focus { border: 1px solid #10b981; }"
    );
    connect(txtBuscar, &QLineEdit::textChanged, this, &VistaVentas::filtrarProductos);
    layoutIzquierdo->addWidget(txtBuscar);

    // Filtros por Categoría
    QHBoxLayout* layoutCategorias = new QHBoxLayout();
    layoutCategorias->setSpacing(8);

    QStringList categorias = {"Todos", "Snacks", "Bebidas", "Comida", "Útiles", "Higiene"};

    for (const QString& cat : categorias) {
        QPushButton* btnCat = new QPushButton(cat, this);
        btnCat->setCheckable(true);
        if (cat == "Todos") btnCat->setChecked(true);

        if (cat == "Todos") {
            btnCat->setStyleSheet("QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border: none; border-radius: 8px; padding: 8px 16px; font-size: 12px; }");
        } else {
            btnCat->setStyleSheet("QPushButton { background-color: #ffffff; color: #4b5563; font-weight: 500; border: 1px solid #e5e7eb; border-radius: 8px; padding: 8px 16px; font-size: 12px; } QPushButton:hover { background-color: #f3f4f6; }");
        }

        connect(btnCat, &QPushButton::clicked, [this, cat]() {
            cambiarCategoriaSlot(cat);
        });

        layoutCategorias->addWidget(btnCat);
    }
    layoutCategorias->addStretch();
    layoutIzquierdo->addLayout(layoutCategorias);

    // Grid de Productos
    QScrollArea* scrollProductos = new QScrollArea(this);
    scrollProductos->setWidgetResizable(true);
    scrollProductos->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget* contenidoGrid = new QWidget();
    gridProductos = new QGridLayout(contenidoGrid);
    gridProductos->setSpacing(15);

    scrollProductos->setWidget(contenidoGrid);
    layoutIzquierdo->addWidget(scrollProductos);

    layoutPrincipal->addWidget(panelIzquierdo, 7);

    // PANEL DERECHO: Detalle de Orden
    QFrame* panelDerecho = new QFrame(this);
    panelDerecho->setStyleSheet("QFrame { background-color: #ffffff; border-radius: 16px; border: 1px solid #e5e7eb; }");
    QVBoxLayout* layoutDerecho = new QVBoxLayout(panelDerecho);
    layoutDerecho->setContentsMargins(18, 18, 18, 18);
    layoutDerecho->setSpacing(10);

    QHBoxLayout* layoutHeaderOrden = new QHBoxLayout();
    QLabel* lblTituloOrden = new QLabel("Mi Orden", this);
    lblTituloOrden->setStyleSheet("font-size: 18px; font-weight: bold; color: #111827; border: none;");

    btnVaciarCarrito = new QPushButton("Vaciar ", this);
    btnVaciarCarrito->setStyleSheet(
        "QPushButton { background: transparent; color: #ef4444; font-size: 11px; font-weight: bold; border: none; }"
        "QPushButton:hover { text-decoration: underline; }"
    );
    connect(btnVaciarCarrito, &QPushButton::clicked, this, &VistaVentas::vaciarCarrito);

    layoutHeaderOrden->addWidget(lblTituloOrden);
    layoutHeaderOrden->addStretch();
    layoutHeaderOrden->addWidget(btnVaciarCarrito);
    layoutDerecho->addLayout(layoutHeaderOrden);

    lblResumenItems = new QLabel("0 artículos en la orden", this);
    lblResumenItems->setStyleSheet("color: #6b7280; font-size: 11px; border: none;");
    layoutDerecho->addWidget(lblResumenItems);

    QScrollArea* scrollCarrito = new QScrollArea(this);
    scrollCarrito->setWidgetResizable(true);
    scrollCarrito->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    contenedorCarrito = new QWidget();
    layoutListaCarrito = new QVBoxLayout(contenedorCarrito);
    layoutListaCarrito->setSpacing(8);
    layoutListaCarrito->setContentsMargins(0, 0, 0, 0);

    scrollCarrito->setWidget(contenedorCarrito);
    layoutDerecho->addWidget(scrollCarrito);

    QFrame* linea = new QFrame(this);
    linea->setFrameShape(QFrame::HLine);
    linea->setStyleSheet("color: #e5e7eb;");
    layoutDerecho->addWidget(linea);

    // Desglose
    auto agregarFilaDesglose = [this, layoutDerecho](const QString& etiqueta, QLabel*& lblValor) {
        QHBoxLayout* layoutFila = new QHBoxLayout();
        QLabel* lblEtiqueta = new QLabel(etiqueta, this);
        lblEtiqueta->setStyleSheet("color: #6b7280; font-size: 12px; border: none;");
        lblValor = new QLabel("₡ 0.00", this);
        lblValor->setStyleSheet("color: #374151; font-weight: 500; font-size: 12px; border: none;");

        layoutFila->addWidget(lblEtiqueta);
        layoutFila->addStretch();
        layoutFila->addWidget(lblValor);
        layoutDerecho->addLayout(layoutFila);
    };

    agregarFilaDesglose("Subtotal", lblSubtotal);
    agregarFilaDesglose("IVA (13%)", lblIVA);

    // Configuración del Descuento
    QHBoxLayout* layoutDesc = new QHBoxLayout();
    QLabel* lblEtiquetaDesc = new QLabel("Descuento:", this);
    lblEtiquetaDesc->setStyleSheet("color: #6b7280; font-size: 12px; border: none;");

    comboTipoDescuento = new QComboBox(this);
    comboTipoDescuento->addItems({"Monto (₡)", "Porcentaje (%)"});
    comboTipoDescuento->setStyleSheet(
        "QComboBox { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 6px; "
        "padding: 2px 6px; font-size: 11px; color: #111827; }"
        "QComboBox::drop-down { border: none; }"
    );

    spinDescuento = new QDoubleSpinBox(this);
    spinDescuento->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spinDescuento->setRange(0.0, 1000000.0);
    spinDescuento->setDecimals(2);
    spinDescuento->setStyleSheet(
        "QDoubleSpinBox { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 6px; "
        "padding: 3px 8px; font-size: 12px; color: #111827; font-weight: bold; }"
        "QDoubleSpinBox:focus { border: 1px solid #10b981; }"
    );

    connect(comboTipoDescuento, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        if (index == 1) {
            spinDescuento->setRange(0.0, 100.0);
        } else {
            spinDescuento->setRange(0.0, 1000000.0);
        }
        actualizarTotales();
    });
    connect(spinDescuento, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VistaVentas::actualizarTotales);

    layoutDesc->addWidget(lblEtiquetaDesc);
    layoutDesc->addWidget(comboTipoDescuento);
    layoutDesc->addWidget(spinDescuento);
    layoutDerecho->addLayout(layoutDesc);

    // Total a Pagar
    QHBoxLayout* layoutTotal = new QHBoxLayout();
    QLabel* lblTextTotal = new QLabel("Total a Pagar", this);
    lblTextTotal->setStyleSheet("color: #111827; font-weight: bold; font-size: 15px; border: none;");
    lblTotal = new QLabel("₡ 0.00", this);
    lblTotal->setStyleSheet("color: #10b981; font-weight: bold; font-size: 18px; border: none;");
    layoutTotal->addWidget(lblTextTotal);
    layoutTotal->addStretch();
    layoutTotal->addWidget(lblTotal);
    layoutDerecho->addLayout(layoutTotal);

    // Panel Solo Efectivo
    QLabel* lblPago = new QLabel("Pago en Efectivo", this);
    lblPago->setStyleSheet("font-size: 13px; font-weight: bold; color: #111827; border: none; margin-top: 4px;");
    layoutDerecho->addWidget(lblPago);

    QWidget* panelEfectivo = new QWidget(this);
    QVBoxLayout* layoutEfectivo = new QVBoxLayout(panelEfectivo);
    layoutEfectivo->setContentsMargins(0, 0, 0, 0);
    layoutEfectivo->setSpacing(6);

    QHBoxLayout* layoutMonto = new QHBoxLayout();
    QLabel* lblPagaCon = new QLabel("Paga con: ₡", panelEfectivo);
    lblPagaCon->setStyleSheet("font-size: 12px; color: #374151; font-weight: bold; border: none;");

    spinMontoPaga = new QDoubleSpinBox(panelEfectivo);
    spinMontoPaga->setButtonSymbols(QAbstractSpinBox::NoButtons);
    spinMontoPaga->setRange(0.0, 1000000.0);
    spinMontoPaga->setDecimals(2);
    spinMontoPaga->setStyleSheet(
        "QDoubleSpinBox { background-color: #ffffff; border: 1px solid #d1d5db; border-radius: 6px; "
        "padding: 5px 8px; font-size: 13px; font-weight: bold; color: #111827; }"
        "QDoubleSpinBox:focus { border: 1px solid #10b981; }"
    );

    layoutMonto->addWidget(lblPagaCon);
    layoutMonto->addWidget(spinMontoPaga);
    layoutEfectivo->addLayout(layoutMonto);

    QHBoxLayout* layoutCambio = new QHBoxLayout();
    QLabel* lblTextoCambio = new QLabel("Vuelto / Cambio:", panelEfectivo);
    lblTextoCambio->setStyleSheet("font-size: 12px; color: #6b7280; border: none;");
    lblCambio = new QLabel("₡ 0.00", panelEfectivo);
    lblCambio->setStyleSheet("font-size: 13px; font-weight: bold; color: #10b981; border: none;");

    layoutCambio->addWidget(lblTextoCambio);
    layoutCambio->addStretch();
    layoutCambio->addWidget(lblCambio);
    layoutEfectivo->addLayout(layoutCambio);

    layoutDerecho->addWidget(panelEfectivo);

    connect(spinMontoPaga, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VistaVentas::calcularCambioEfectivo);

    btnCheckout = new QPushButton("Procesar Venta", this);
    btnCheckout->setStyleSheet(
        "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; font-size: 14px; "
        "border: none; border-radius: 10px; padding: 12px; }"
        "QPushButton:hover { background-color: #059669; }"
        "QPushButton:pressed { background-color: #047857; }"
    );
    connect(btnCheckout, &QPushButton::clicked, this, &VistaVentas::procesarCheckout);
    layoutDerecho->addWidget(btnCheckout);

    layoutPrincipal->addWidget(panelDerecho, 3);
}

void VistaVentas::cambiarCategoriaSlot(const QString& cat) {
    categoriaSeleccionada = cat;
    filtrarProductos();
}

void VistaVentas::filtrarProductos() {
    renderizarProductos();
}

void VistaVentas::renderizarProductos() {
    QLayoutItem* item;
    while ((item = gridProductos->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    QString filtroTexto = txtBuscar->text().trimmed().toLower();
    int fila = 0;
    int columna = 0;

    for (const auto& prod : catalogo) {
        bool coincideCategoria = (categoriaSeleccionada == "Todos" || prod.categoria == categoriaSeleccionada);
        bool coincideTexto = filtroTexto.isEmpty() ||
                              prod.nombre.toLower().contains(filtroTexto) ||
                              prod.id.toLower().contains(filtroTexto);

        if (coincideCategoria && coincideTexto) {
            gridProductos->addWidget(crearTarjetaProducto(prod), fila, columna);
            columna++;
            if (columna >= 3) {
                columna = 0;
                fila++;
            }
        }
    }
}

QWidget* VistaVentas::crearTarjetaProducto(const ProductoItem& prod) {
    QFrame* tarjeta = new QFrame();
    tarjeta->setStyleSheet(
        "QFrame { background-color: #ffffff; border-radius: 12px; border: 1px solid #e5e7eb; }"
        "QFrame:hover { border: 1px solid #10b981; }"
    );
    QVBoxLayout* layout = new QVBoxLayout(tarjeta);
    layout->setContentsMargins(12, 12, 12, 12);

    QLabel* lblNombre = new QLabel(prod.nombre, tarjeta);
    lblNombre->setStyleSheet("font-weight: bold; font-size: 13px; color: #111827; border: none;");

    QLabel* lblDesc = new QLabel(QString("%1 | Stock: %2").arg(prod.descripcion).arg(prod.stock), tarjeta);
    if (prod.stock <= 3) {
        lblDesc->setStyleSheet("color: #ef4444; font-weight: bold; font-size: 11px; border: none;");
    } else {
        lblDesc->setStyleSheet("color: #6b7280; font-size: 11px; border: none;");
    }

    QHBoxLayout* layoutBottom = new QHBoxLayout();
    QLabel* lblPrecio = new QLabel(QString("₡ %1").arg(prod.precio, 0, 'f', 2), tarjeta);
    lblPrecio->setStyleSheet("color: #10b981; font-weight: bold; font-size: 14px; border: none;");

    QPushButton* btnAgregar = new QPushButton(" + ", tarjeta);

    if (prod.stock > 0) {
        btnAgregar->setStyleSheet(
            "QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; "
            "border-radius: 6px; border: none; padding: 4px 10px; font-size: 12px; }"
            "QPushButton:hover { background-color: #059669; }"
        );
        connect(btnAgregar, &QPushButton::clicked, [this, prod]() {
            agregarAlCarrito(prod);
        });
    } else {
        btnAgregar->setText("Agotado");
        btnAgregar->setEnabled(false);
        btnAgregar->setStyleSheet(
            "QPushButton:disabled { background-color: #f3f4f6; color: #6b7280; font-size: 11px; "
            "font-weight: bold; border-radius: 6px; border: 1px solid #e5e7eb; padding: 4px 8px; }"
        );
    }

    layoutBottom->addWidget(lblPrecio);
    layoutBottom->addStretch();
    layoutBottom->addWidget(btnAgregar);

    layout->addWidget(lblNombre);
    layout->addWidget(lblDesc);
    layout->addSpacing(5);
    layout->addLayout(layoutBottom);

    return tarjeta;
}

void VistaVentas::agregarAlCarrito(const ProductoItem& prod) {
    int cantidadEnCarrito = 0;
    for (const auto& item : carrito) {
        if (item.producto.id == prod.id) {
            cantidadEnCarrito = item.cantidad;
            break;
        }
    }

    if (cantidadEnCarrito + 1 > prod.stock) {
        QMessageBox::warning(this, "Stock Insuficiente",
            QString("No se puede agregar más unidades de '%1'. Stock disponible: %2").arg(prod.nombre).arg(prod.stock));
        return;
    }

    for (auto& item : carrito) {
        if (item.producto.id == prod.id) {
            item.cantidad++;
            renderizarCarrito();
            actualizarTotales();
            return;
        }
    }

    carrito.push_back({prod, 1});
    renderizarCarrito();
    actualizarTotales();
}

void VistaVentas::renderizarCarrito() {
    QLayoutItem* child;
    while ((child = layoutListaCarrito->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }

    int totalArticulos = 0;

    if (carrito.empty()) {
        QLabel* lblVacio = new QLabel("El carrito está vacío", contenedorCarrito);
        lblVacio->setAlignment(Qt::AlignCenter);
        lblVacio->setStyleSheet("color: #9ca3af; font-size: 13px; padding: 20px; border: none;");
        layoutListaCarrito->addWidget(lblVacio);
    } else {
        for (size_t i = 0; i < carrito.size(); ++i) {
            layoutListaCarrito->addWidget(crearWidgetCarritoItem(carrito[i], static_cast<int>(i)));
            totalArticulos += carrito[i].cantidad;
        }
    }

    lblResumenItems->setText(QString("%1 artículos en la orden").arg(totalArticulos));
    layoutListaCarrito->addStretch();
}

QWidget* VistaVentas::crearWidgetCarritoItem(const ItemCarrito& item, int index) {
    QWidget* w = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(w);
    layout->setContentsMargins(0, 4, 0, 4);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    QLabel* lblNombre = new QLabel(item.producto.nombre, w);
    lblNombre->setStyleSheet("font-weight: bold; font-size: 12px; color: #111827; border: none;");

    double precioTotalItem = item.producto.precio * item.cantidad;
    QLabel* lblSub = new QLabel(QString("₡ %1").arg(precioTotalItem, 0, 'f', 2), w);
    lblSub->setStyleSheet("color: #10b981; font-weight: bold; font-size: 12px; border: none;");

    infoLayout->addWidget(lblNombre);
    infoLayout->addWidget(lblSub);

    QHBoxLayout* controlesCant = new QHBoxLayout();
    QPushButton* btnMenos = new QPushButton("-", w);
    QLabel* lblCant = new QLabel(QString::number(item.cantidad), w);
    QPushButton* btnMas = new QPushButton("+", w);

    QString estiloBtnCant =
        "QPushButton { background-color: #f3f4f6; color: #111827; font-weight: bold; "
        "border-radius: 4px; border: none; padding: 2px 8px; font-size: 12px; }"
        "QPushButton:hover { background-color: #e5e7eb; }";

    btnMenos->setStyleSheet(estiloBtnCant);
    btnMas->setStyleSheet(estiloBtnCant);
    lblCant->setStyleSheet("font-size: 12px; font-weight: bold; color: #111827; padding: 0 4px; border: none;");

    connect(btnMenos, &QPushButton::clicked, [this, index]() { cambiarCantidadItem(index, -1); });
    connect(btnMas, &QPushButton::clicked, [this, index]() { cambiarCantidadItem(index, 1); });

    controlesCant->addWidget(btnMenos);
    controlesCant->addWidget(lblCant);
    controlesCant->addWidget(btnMas);

    QPushButton* btnEliminar = new QPushButton("", w);
    btnEliminar->setStyleSheet("QPushButton { border: none; color: #ef4444; font-size: 13px; background: transparent; }");
    connect(btnEliminar, &QPushButton::clicked, [this, index]() { eliminarDelCarrito(index); });

    layout->addLayout(infoLayout);
    layout->addStretch();
    layout->addLayout(controlesCant);
    layout->addSpacing(8);
    layout->addWidget(btnEliminar);

    return w;
}

void VistaVentas::cambiarCantidadItem(int index, int delta) {
    if (index >= 0 && index < static_cast<int>(carrito.size())) {
        if (delta > 0) {
            if (carrito[index].cantidad + 1 > carrito[index].producto.stock) {
                QMessageBox::warning(this, "Stock Insuficiente",
                    QString("No hay más stock de '%1'. Máximo disponible: %2")
                        .arg(carrito[index].producto.nombre)
                        .arg(carrito[index].producto.stock));
                return;
            }
        }

        carrito[index].cantidad += delta;
        if (carrito[index].cantidad <= 0) {
            carrito.erase(carrito.begin() + index);
        }
        renderizarCarrito();
        actualizarTotales();
    }
}

void VistaVentas::eliminarDelCarrito(int index) {
    if (index >= 0 && index < static_cast<int>(carrito.size())) {
        carrito.erase(carrito.begin() + index);
        renderizarCarrito();
        actualizarTotales();
    }
}

void VistaVentas::vaciarCarrito() {
    if (carrito.empty()) return;

    if (QMessageBox::question(this, "Vaciar Orden", "¿Está seguro de que desea limpiar todos los productos de la orden?",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        carrito.clear();
        spinDescuento->setValue(0.0);
        spinMontoPaga->setValue(0.0);
        renderizarCarrito();
        actualizarTotales();
    }
}

void VistaVentas::actualizarTotales() {
    double sumaPreciosBrutos = 0.0;
    for (const auto& item : carrito) {
        sumaPreciosBrutos += item.producto.precio * item.cantidad;
    }

    double valorDescuento = spinDescuento->value();
    if (comboTipoDescuento->currentIndex() == 1) { // Porcentaje (%)
        descuentoActual = sumaPreciosBrutos * (valorDescuento / 100.0);
    } else { // Monto fijo (₡)
        descuentoActual = valorDescuento;
    }

    double subtotalConDescuento = std::max(0.0, sumaPreciosBrutos - descuentoActual);

    subtotalActual = subtotalConDescuento / 1.13;
    ivaActual = subtotalConDescuento - subtotalActual;
    totalActual = subtotalConDescuento;

    lblSubtotal->setText(QString("₡ %1").arg(subtotalActual, 0, 'f', 2));
    lblIVA->setText(QString("₡ %1").arg(ivaActual, 0, 'f', 2));
    lblTotal->setText(QString("₡ %1").arg(totalActual, 0, 'f', 2));

    calcularCambioEfectivo();
}

void VistaVentas::calcularCambioEfectivo() {
    double pagaCon = spinMontoPaga->value();
    double cambio = pagaCon - totalActual;

    if (totalActual == 0.0) {
        lblCambio->setText("₡ 0.00");
        lblCambio->setStyleSheet("font-size: 13px; font-weight: bold; color: #10b981; border: none;");
    } else if (cambio < 0.0) {
        lblCambio->setText("Falta Dinero");
        lblCambio->setStyleSheet("font-size: 12px; font-weight: bold; color: #ef4444; border: none;");
    } else {
        lblCambio->setText(QString("₡ %1").arg(cambio, 0, 'f', 2));
        lblCambio->setStyleSheet("font-size: 13px; font-weight: bold; color: #10b981; border: none;");
    }
}

void VistaVentas::procesarCheckout() {
    if (carrito.empty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Orden Vacía");
        msg.setText("Debe agregar al menos un producto antes de procesar la venta.");
        msg.setIcon(QMessageBox::Warning);
        msg.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border-radius: 6px; padding: 6px 15px; }");
        msg.exec();
        return;
    }

    double pagaCon = spinMontoPaga->value();
    if (pagaCon < totalActual) {
        double falta = totalActual - pagaCon;

        QMessageBox msg(this);
        msg.setWindowTitle("Dinero Insuficiente");

        QString mensajeError = QString(
            "<b>El dinero ingresado es insuficiente.</b><br><br>"
            "• <b>Total:</b> ₡ %1<br>"
            "• <b>Paga con:</b> ₡ %2<br>"
            "• <font color='#ef4444'><b>Hace falta: ₡ %3</b></font><br><br>"
            "¿Desea regresar a revisar la orden o cancelar el pedido?"
        ).arg(totalActual, 0, 'f', 2).arg(pagaCon, 0, 'f', 2).arg(falta, 0, 'f', 2);

        msg.setText(mensajeError);
        msg.setIcon(QMessageBox::Warning);

        QPushButton* btnRegresar = msg.addButton("Regresar a Mi Orden", QMessageBox::AcceptRole);
        QPushButton* btnCancelar = msg.addButton("Cancelar Pedido", QMessageBox::RejectRole);

        msg.setStyleSheet("QMessageBox { background-color: #ffffff; } QLabel { color: #111827; font-size: 13px; } QPushButton { background-color: #f3f4f6; color: #111827; font-weight: bold; border-radius: 6px; padding: 6px 12px; font-size: 11px; }");
        msg.exec();

        if (msg.clickedButton() == btnCancelar) {
            carrito.clear();
            spinDescuento->setValue(0.0);
            spinMontoPaga->setValue(0.0);
            renderizarCarrito();
            actualizarTotales();
        }
        return;
    }

    // 1. Rebaja única de inventario en memoria
    for (const auto& item : carrito) {
        for (auto& prodCat : catalogo) {
            if (prodCat.id == item.producto.id) {
                prodCat.stock -= item.cantidad;
                break;
            }
        }
    }

    // 2. Datos del Ticket
    DatosVentaTicket datos;
    datos.idVenta = QString("V-%1").arg(contadorVentas++);
    datos.cajero = "Cajero Turno UNA";
    datos.fechaHora = QDateTime::currentDateTime();
    datos.subtotal = subtotalActual;
    datos.iva = ivaActual;
    datos.total = totalActual;
    datos.pagaCon = pagaCon;
    datos.cambio = pagaCon - totalActual;

    for (const auto& item : carrito) {
        datos.items.push_back({item.producto.nombre, item.cantidad, item.producto.precio, item.producto.precio * item.cantidad});
    }

    // 3. Persistir en disco (Archivos JSON)
    GestorJSON gestor;
    gestor.guardarProductos(catalogo); // Guarda el inventario con el stock rebajado
    gestor.registrarVenta(datos);     // Registra la transacción completa en ventas.json

    // 4. Mostrar ticket en pantalla y generar archivo TXT
    mostrarYGuardarTicket(datos);

    // 5. Reiniciar orden y limpiar campos
    carrito.clear();
    spinDescuento->setValue(0.0);
    spinMontoPaga->setValue(0.0);
    renderizarProductos();
    renderizarCarrito();
    actualizarTotales();
}

void VistaVentas::mostrarYGuardarTicket(const DatosVentaTicket& datos) {
    std::unique_ptr<IGeneradorTicket> generadorTXT = std::make_unique<GeneradorTicketTXT>();
    std::unique_ptr<IGeneradorTicket> generadorHTML = std::make_unique<GeneradorTicketHTML>();

    QString contenidoTXT = generadorTXT->generarContenido(datos);
    QString contenidoHTML = generadorHTML->generarContenido(datos);

    QString nombreArchivo = QString("Ticket_%1%2").arg(datos.idVenta, generadorTXT->obtenerExtension());
    QFile file(nombreArchivo);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << contenidoTXT;
        file.close();
    }

    QDialog dlgTicket(this);
    dlgTicket.setWindowTitle("Ticket de Compra - UNA Pulpe");
    dlgTicket.setFixedSize(360, 540);
    dlgTicket.setStyleSheet("background-color: #ffffff;");

    QVBoxLayout* layoutDlg = new QVBoxLayout(&dlgTicket);
    layoutDlg->setContentsMargins(15, 15, 15, 15);

    QTextBrowser* textViewer = new QTextBrowser(&dlgTicket);
    textViewer->setHtml(contenidoHTML);
    textViewer->setStyleSheet("QTextBrowser { border: 1px solid #e5e7eb; border-radius: 8px; background-color: #ffffff; color: #111827; padding: 10px; }");

    QPushButton* btnCerrar = new QPushButton("Aceptar", &dlgTicket);
    btnCerrar->setStyleSheet("QPushButton { background-color: #10b981; color: #ffffff; font-weight: bold; border: none; border-radius: 8px; padding: 10px; font-size: 13px; }"
                             "QPushButton:hover { background-color: #059669; }");
    connect(btnCerrar, &QPushButton::clicked, &dlgTicket, &QDialog::accept);

    layoutDlg->addWidget(textViewer);
    layoutDlg->addSpacing(10);
    layoutDlg->addWidget(btnCerrar);

    dlgTicket.exec();
}
