/*
 * Created by SharpDevelop.
 * User: Administrator
 * Date: 16/01/2005
 * Time: 01:19
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */

using System;
using System.Drawing;
using System.Windows.Forms;

namespace MelkorNETGUI
{
//	public class ClosingEventArgs : EventArgs {
//		public bool cancel = false;
//		public ClosingEventArgs() {}
//	}
	
//	public delegate void ClosingEventHandler(object source, ClosingEventArgs args);
	
	/// <summary>
	/// Description of Form1.
	/// </summary>
	public class LogWindow : System.Windows.Forms.Form
	{
//		public event ClosingEventHandler Closing;
		public System.Windows.Forms.TextBox logWindowTextBox;
		public LogWindow(System.ComponentModel.CancelEventHandler customClosingHandler)
		{
			//
			// The InitializeComponent() call is required for Windows Forms designer support.
			//
			InitializeComponent();
			this.Closing += new System.ComponentModel.CancelEventHandler(customClosingHandler);
		}
		
		public void writeLine(string text)
		{
			logWindowTextBox.AppendText(text + Environment.NewLine);
		}
		
		#region Windows Forms Designer generated code
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent() {
			this.logWindowTextBox = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// logWindowTextBox
			// 
			this.logWindowTextBox.AcceptsReturn = true;
			this.logWindowTextBox.AcceptsTab = true;
			this.logWindowTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
						| System.Windows.Forms.AnchorStyles.Left) 
						| System.Windows.Forms.AnchorStyles.Right)));
			this.logWindowTextBox.Location = new System.Drawing.Point(0, 0);
			this.logWindowTextBox.Multiline = true;
			this.logWindowTextBox.Name = "logWindowTextBox";
			this.logWindowTextBox.ReadOnly = true;
			this.logWindowTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.logWindowTextBox.Size = new System.Drawing.Size(656, 432);
			this.logWindowTextBox.TabIndex = 0;
			this.logWindowTextBox.Text = "";
			// 
			// LogWindow
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(656, 429);
			this.Controls.Add(this.logWindowTextBox);
			this.Name = "LogWindow";
			this.Text = "Log Window";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.LogWindowClosing);
			this.ResumeLayout(false);
		}
		#endregion
		void LogWindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			// Stop the close button disposing of the Form resources
			e.Cancel = true;
			
			// Fire a logWindowClosing event to the parent form
//			if(Closing != null) {
//				ClosingEventArgs args = new ClosingEventArgs();
//				Closing(this, args);
//				if(args.cancel)
//					return;
//			}
		}	
	}
}
