from django.urls import path
from django.urls import include, re_path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('izquierda', views.ajax_izquierda, name='ajax_izquierda'),

]