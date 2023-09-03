//
//  ContentView.swift
//  WatchQuake2 Watch App
//
//  Created by ByteOverlord on 26.11.2022.
//

import SwiftUI
import UIKit

class Model : ObservableObject {
    @Published var txt = String("")
    @Published var benchmarkTxt = String("")
    @Published var img : CGImage!
    init() {
        img = nil
        txt = String("")
        benchmarkTxt = String("")
    }
}

let myModel = Model()

@_cdecl("refresh_screen")
func refreshScreen(ptr: UnsafeRawPointer) -> () {
    myModel.img = WQCreateGameImage()!.takeRetainedValue()
    myModel.benchmarkTxt = String(cString: WQGetBenchmarkString())
    if (WQShowFPS() != 0)
    {
        myModel.txt = String(cString: WQGetStatsString())
    }
    else
    {
        myModel.txt = String("")
    }
}

struct ImageOverlay: View {
    @ObservedObject var model: Model
    init() {
        model = myModel
    }
    var body: some View {
        Text(model.txt).font(.system(size: 6)).fixedSize().padding(Edge.Set(Edge.top), Double(-8.0))
        Text(model.benchmarkTxt).font(.system(size: 6)).fixedSize().padding(Edge.Set(Edge.top), Double(8.0))
    }
}

struct ContentView: View {
    @State var isTapped = false
    @ObservedObject var model: Model
    init()
    {
        model = myModel
    }
    var body: some View {
        let stats = WQGetStats()
        //let scaleX = (Float(stats.width) / Float(stats.devWidth))
        //let scaleY = (Float(stats.height) / Float(stats.devHeight))
        let scale = stats.devPixelsPerDot//scaleY > scaleX ? scaleY : scaleX
        if (model.img != nil)
        {
            //let offset = CGSize(width: 0.0, height: 128.0)
            Image.init(model.img, scale: Double(scale), label: Text("Game"))
                //.scaledToFit()
                .overlay(ImageOverlay(), alignment: .top)
        }
    }
}

/*struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}*/
