from django.urls import path
from django.urls import include, re_path
from django.views.generic.base import RedirectView
from . import views

favicon_view = RedirectView.as_view(url='/static/favicon.png', permanent=True)

urlpatterns = [
    path('', views.index, name='index'),
    path('favicon.ico', favicon_view, name='favicon'),
    path('izquierda', views.ajax_izquierda, name='ajax_izquierda'),
    path('cambiarEstado', views.cambiarEstado, name='cambiarEstado'),
    path('accionBrazo', views.accionBrazo, name='accionBrazo'),
    path('rpi', views.rpi, name='rpi'),
    path('actualizarRPM', views.actualizarRPM, name='actualizarRPM'),
    


]