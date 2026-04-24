package com.example.musicwiki.screens

sealed class Screen(val route:String) {
    data object GenericScreen: Screen("GenericScreen")
    data object GenericDetailScreen: Screen("GenericDetailScreen")

    fun withArgs(vararg args:String):String{

        return buildString {
            append(route)
            args.forEach {
                arg->
                append("/${arg}")
            }
        }
    }

}